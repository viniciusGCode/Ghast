#include <ghast/windows/process_memory.hpp>

#include <Windows.h>

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

} // namespace

namespace ghast::windows {
  
  std::vector<std::byte> read_process_memory(
  std::uint32_t process_id,
  std::uintptr_t address,
  std::size_t size)
{

  const UniqueHandle process{
    OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, process_id)
  };

  if(process.get() == nullptr) {
    throw std::system_error{
      static_cast<int>(GetLastError()),
      std::system_category(),
      "Failed to open process for reading memory"
    };
  }

  std::vector<std::byte> buffer;

  buffer.resize(size);

  SIZE_T bytes_read = 0;

  if(!ReadProcessMemory(
    process.get(),
    reinterpret_cast<LPCVOID>(address),
    buffer.data(),
    buffer.size(),
    &bytes_read)) {
    throw std::system_error{
      static_cast<int>(GetLastError()),
      std::system_category(),
      "Failed to read process memory"
    };
  }

  buffer.resize(bytes_read);
  return buffer;
}

} // namespace ghast::windows
