#pragma once
#include <algorithm>  // needed for all_of to work for some reason??
#include <cctype>
#include <charconv>
#include <format>
#include <string>
#include <sstream>
#include <optional>
#include <ranges>

namespace csopesy {
  // === String Aliases ===
  using std::string;
  using str = std::string;
  using Str = std::optional<string>;

  // === Character Utilities ===
  using std::isalpha;
  using std::isalnum;

  // === Stream Aliases ===
  using std::stringstream;
  using std::istringstream;
  using std::ostringstream;
  using sstream = std::stringstream;
  using isstream = std::istringstream;
  using osstream = std::ostringstream;

  // === Formatting & Parsing ===
  using std::format;
  using std::errc;
  using std::from_chars;

  // === Constants ===
  constexpr auto NPOS = string::npos;

  // === Validation ===
  inline bool is_num(const std::string& s) {
    return !s.empty() && std::ranges::all_of(s, [](char c) {
      return std::isdigit(static_cast<unsigned char>(c));
    });
  }
}
