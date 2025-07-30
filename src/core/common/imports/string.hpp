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

// === Utilities ===
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

// === Utility functions ===

/** @brief Returns a lowercase copy of the given string. */
auto lowercase(str raw) -> str {
  for (char& c: raw)
    c = tolower(c);
  return raw;
}

/** @brief Joins a vector of strings using the given delimiter. */
auto join(vec<str>& args, char delim=' ') -> str {
  if (args.empty()) return "";
  
  auto oss = osstream();
  oss << args[0];
  for (auto i = 1u; i < args.size(); ++i)
    oss << delim << args[i];
  return oss.str();
}

/** @brief Returns a copy of the string with leading and trailing whitespace removed. */
auto trim(const str& s) -> str {
  auto start = s.find_first_not_of(" \t\n\r");
  if (start == NPOS) return "";  // all whitespace
  auto end = s.find_last_not_of(" \t\n\r");
  return s.substr(start, end - start + 1);
};
