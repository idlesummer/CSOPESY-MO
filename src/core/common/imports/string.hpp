#pragma once
#include <algorithm>  // needed for all_of to work for some reason??
#include <cctype>
#include <charconv>
#include <format>
#include <string>
#include <sstream>
#include <optional>
#include <ranges>


// === String Aliases ===
using std::string;
using str = std::string;
using Str = std::optional<string>;
using std::literals::string_literals::operator""s;

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
using std::errc;
using std::format;
using std::from_chars;

// === Constants ===
constexpr auto NPOS = string::npos;

// === Validation ===

/** @brief Returns true if the string consists only of digit characters (0–9). */
auto is_digits(const std::string& s) -> bool {
  return !s.empty() && std::ranges::all_of(s, [](char c) {
    return std::isdigit(static_cast<unsigned char>(c));
  });
}

// === Case conversions ===

/** @brief Returns a lowercase copy of the given string. */
auto lowercase(str raw) -> str {
  for (char& c: raw)
    c = tolower(c);
  return raw;
}
