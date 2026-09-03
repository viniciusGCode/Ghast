#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ghast::windows{

  struct ProcessSummary{
    std::uint32_t process_id;
    std::wstring executable_name;
    std::optional<std::wstring> executable_path;
  };

  [[nodiscard]] std::vector<ProcessSummary> enumerate_processes();

} // namespace ghast::windows