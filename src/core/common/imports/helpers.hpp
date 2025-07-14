#pragma once
#include <cmath>
#include <cstdint>
#include <string>
#include <sstream>

namespace csopesy {
  
  // === Utility functions ===
  constexpr inline uint32_t count_digits(uint32_t n) {
    return (n == 0) ? 1 : static_cast<int>(std::log10(n)) + 1;
  }

  constexpr inline std::string pad_left(const std::string& s, uint32_t width, char fill = ' ') {
    return std::string(width > s.length() ? width - s.length() : 0, fill) + s;
  }
}
