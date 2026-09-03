#include <ghast/windows/process_snapshot.hpp>
#include <ghast/windows/process_memory.hpp>

#include <cwctype>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

std::wstring to_lowercase(std::wstring value)
{
    for (auto& character : value) {
        character = static_cast<wchar_t>(std::towlower(character));
    }

    return value;
}

bool matches_filter(
    const ghast::windows::ProcessSummary& process,
    const std::optional<std::wstring>& process_name_filter)
{
    if (!process_name_filter.has_value()) {
        return true;
    }

    const auto executable_name = to_lowercase(process.executable_name);

    return executable_name.find(*process_name_filter) != std::wstring::npos;
}

void print_process(const ghast::windows::ProcessSummary& process)
{
    std::wcout << L"Process ID: " << process.process_id
               << L", Executable Name: " << process.executable_name;
    if (process.executable_path.has_value()) {
        std::wcout << L", Executable Path: " << *process.executable_path;
    }

    std::wcout << L'\n';
}

void print_bytes(const std::vector<std::byte>& bytes)
{
    for (const auto byte : bytes) {
        const auto value = static_cast<unsigned int>(byte);
        std::wcout << std::hex << std::setw(2) << std::setfill(L'0')
                   << value << L' ';
    }

    std::wcout << std::dec << L'\n';
}

std::uint32_t parse_process_id(const wchar_t* value)
{
    return static_cast<std::uint32_t>(std::stoul(value, nullptr, 10));
}

std::uintptr_t parse_address(const wchar_t* value)
{
    return static_cast<std::uintptr_t>(std::stoull(value, nullptr, 16));
}

std::size_t parse_size(const wchar_t* value)
{
    return static_cast<std::size_t>(std::stoull(value, nullptr, 10));
}

int run(const std::optional<std::wstring>& process_name_filter)
{
    const auto processes = ghast::windows::enumerate_processes();

    int match_count = 0;
    for (const auto& process : processes) {
        if (matches_filter(process, process_name_filter)) {
            print_process(process);
            ++match_count;
        }
    }

    if (process_name_filter.has_value() && match_count == 0) {
        std::wcerr << L"No processes found matching filter: "
                   << *process_name_filter << L'\n';
        return 1;
    }

    return 0;
}

int wmain(int argc, wchar_t* argv[])
{

    if(argc >= 5 && std::wstring{argv[1]} == L"read") {
        const auto process_id = parse_process_id(argv[2]);
        const auto address = parse_address(argv[3]);
        const auto size = parse_size(argv[4]);

        try {
            const auto bytes = ghast::windows::read_process_memory(process_id, address, size);
            print_bytes(bytes);
            return 0;
        } catch (const std::system_error& error) {
            std::cerr << "Error reading process memory: " << error.code().value() << '\n';
            return 2;
        }
    }

    const std::optional<std::wstring> process_name_filter =
        argc >= 2
            ? std::optional<std::wstring>{to_lowercase(argv[1])}
            : std::nullopt;

    try {
        return run(process_name_filter);
    } catch (const std::system_error& error) {
        std::cerr << "Windows API error: " << error.code().value() << '\n';
        return 2;
    }
}
