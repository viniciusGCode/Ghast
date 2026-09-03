#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ghast::windows {
  
  [[nodiscard]] std::vector<std::byte> read_process_memory(
    std::uint32_t process_id,
    std::uintptr_t address,
    std::size_t size);
} // namespace ghast::windows