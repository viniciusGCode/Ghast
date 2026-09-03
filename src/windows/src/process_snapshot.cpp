#include <ghast/windows/process_snapshot.hpp>

#include <Windows.h>
#include <TlHelp32.h>

#include <cstddef>
#include <optional>
#include <string>
#include <system_error>

namespace {

class UniqueHandle {
public:
    explicit UniqueHandle(HANDLE handle) noexcept
        : handle_{handle}
    {
    }

    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;

    ~UniqueHandle()
    {
        if (handle_ != nullptr && handle_ != INVALID_HANDLE_VALUE) {
            CloseHandle(handle_);
        }
    }

    [[nodiscard]] HANDLE get() const noexcept
    {
        return handle_;
    }

private:
    HANDLE handle_;
};

std::optional<std::wstring> query_process_image_path(std::uint32_t process_id)
{
    const UniqueHandle process{
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, process_id)
    };

    if (process.get() == nullptr) {
        return std::nullopt;
    }

    std::wstring buffer;
    buffer.resize(MAX_PATH);

    constexpr std::size_t max_path_buffer = 32768;

    for (;;) {
        DWORD size = static_cast<DWORD>(buffer.size());

        if (QueryFullProcessImageNameW(process.get(), 0, buffer.data(), &size)) {
            buffer.resize(size);
            return buffer;
        }

        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            return std::nullopt;
        }

        if (buffer.size() >= max_path_buffer) {
            return std::nullopt;
        }

        buffer.resize(buffer.size() * 2);
    }
}

}

namespace ghast::windows {

std::vector<ProcessSummary> enumerate_processes()
{
    const UniqueHandle snapshot{
        CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
    };

    if (snapshot.get() == INVALID_HANDLE_VALUE) {
        throw std::system_error{
            std::error_code{
                static_cast<int>(GetLastError()),
                std::system_category()
            },
            "Failed to create process snapshot"
        };
    }

    PROCESSENTRY32W process_entry{};
    process_entry.dwSize = sizeof(process_entry);

    std::vector<ProcessSummary> processes;

    if (!Process32FirstW(snapshot.get(), &process_entry)) {
        return processes;
    }

    do {
        const auto process_id = static_cast<std::uint32_t>(process_entry.th32ProcessID);
        processes.push_back(ProcessSummary{
            process_id,
            process_entry.szExeFile,
            query_process_image_path(process_id)
        });

    } while (Process32NextW(snapshot.get(), &process_entry));

    return processes;
}

} // namespace ghast::windows
