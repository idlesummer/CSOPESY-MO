#pragma once
#include <cmath>
#include <cstdint>
#include <string>
#include <sstream>


// === Utility functions ===
constexpr auto count_digits(uint32_t n) -> uint32_t {
  return (n == 0) ? 1 : static_cast<int>(std::log10(n)) + 1;
}

constexpr auto pad_left(const std::string& s, uint32_t width, char fill=' ') -> std::string {
  return std::string(width > s.length() ? width - s.length() : 0, fill) + s;
}

/**  * @brief Converts a string to uint32. Supports hex (0x...), clamps on overflow, returns 0 on failure. */
auto stoui(const std::string& s) -> std::uint32_t {
  try {
    unsigned long value = std::stoul(s, nullptr, 0); // base 0 = autodetect
    return (value > std::numeric_limits<std::uint32_t>::max())
      ? std::numeric_limits<std::uint32_t>::max()
      : static_cast<std::uint32_t>(value);
  } catch (...) {
    return 0;
  }
}
