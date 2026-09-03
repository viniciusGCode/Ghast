#include <Windows.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

struct LabState {
    std::uint32_t marker;
    std::array<int, 4> values;
    int selected_value;
    std::array<char, 8> tag;
};

void print_address(const char* label, const void* address)
{
    std::cout << label << ": " << address << '\n';
}

std::uintptr_t address_of(const void* pointer)
{
    return reinterpret_cast<std::uintptr_t>(pointer);
}

void print_offset(const char* label, const void* base, const void* field)
{
    std::cout
        << label
        <<" offset: "
        <<(address_of(field) - address_of(base))
        << " bytes\n";
};

int main()
{

    LabState state{
        0x6c6f7665,
        {1337, 2048, 4096, 9001},
        1337,
        {'G', 'H', 'A', 'S', 'T', 'L', 'A', 'B'}
    };

    volatile LabState* observable_state = &state;

    std::cout << "ghast_lab ready" << '\n';
    std::cout << "pid: " << GetCurrentProcessId() << '\n';
    print_address("state address", static_cast<const void*>(&state));
    print_address("marker address", static_cast<const void*>(&state.marker));
    print_address("values[0] address", static_cast<const void*>(&state.values[0]));
    print_address("values[1] address", static_cast<const void*>(&state.values[1]));
    print_address("values[2] address", static_cast<const void*>(&state.values[2]));
    print_address("values[3] address", static_cast<const void*>(&state.values[3]));
    print_address("selected_value address", static_cast<const void*>(&state.selected_value));
    print_address("tag address", static_cast<const void*>(&state.tag));
    print_offset("marker", static_cast<const void*>(&state), static_cast<const void*>(&state.marker));
    print_offset("values[0]", static_cast<const void*>(&state), static_cast<const void*>(&state.values[0]));
    print_offset("values[1]", static_cast<const void*>(&state), static_cast<const void*>(&state.values[1]));
    print_offset("values[2]", static_cast<const void*>(&state), static_cast<const void*>(&state.values[2]));
    print_offset("values[3]", static_cast<const void*>(&state), static_cast<const void*>(&state.values[3]));
    print_offset("selected_value", static_cast<const void*>(&state), static_cast<const void*>(&state.selected_value));
    print_offset("tag", static_cast<const void*>(&state), static_cast<const void*>(&state.tag));
    std::cout << "state size: " << sizeof(state) << " bytes\n";
    std::cout << "commands: set <number>, show, quit\n";

    std::string command;

    while (std::cin >> command) {
        if (command == "quit") {
            break;
        }

        if (command == "show") {
            std::cout << "marker: " << std::hex << observable_state->marker << std::dec << '\n';
            std::cout << "selected_value: " << observable_state->selected_value << '\n';
            continue;
        }

        if (command == "set") {
            int next_value{};
            if(std::cin >> next_value) {
                state.selected_value = next_value;
                state.values[0] = next_value;
                std::cout << "selected_value set to: " << observable_state->selected_value << '\n';
            }
            continue;
        }

        std::cout << "unknown command: " << command << '\n';
    }

    return 0;
}
