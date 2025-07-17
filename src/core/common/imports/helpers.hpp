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
