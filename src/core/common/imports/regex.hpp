#pragma once
#include <regex>
#include <string>
#include <vector>
#include <ranges>
#include <algorithm>


using std::regex;
using std::smatch;
using std::regex_search;
using std::regex_replace;


class re {
  public:

  /** @brief Tokenizes a line into std::strings while preserving quoted substd::strings. */
  static auto tokenize(const std::string& line, bool sanitize=true, bool unescape=true) -> std::vector<std::string> {
    static const auto pattern = regex(R"("([^"\\]|\\.)*"|\S+)");
    auto input = sanitize ? sanitize_quotes(line) : line;

    auto tokens = std::vector<std::string>();
    auto begin = std::sregex_iterator(input.begin(), input.end(), pattern);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it)
      tokens.push_back(unescape ? unquote(it->str()) : it->str());
    return tokens;
  }

  private:
  /** @brief Fixes unmatched unescaped quotes by appending a quote at the end. */
  static auto sanitize_quotes(const std::string& line) -> std::string {
    auto quote_count = 0u;
    for (auto i = 0u; i < line.size(); ++i)
      if (line[i] == '"' && (i == 0 || line[i-1] != '\\'))
        ++quote_count;
    return (quote_count % 2 != 0) ? (line + '"') : line;
  }

  /** @brief Removes surrounding quotes and unescapes escaped characters. */
  static auto unquote(const std::string& quoted) -> std::string {
    if (quoted.size() < 2 || quoted.front() != '"' || quoted.back() != '"')
      return quoted;

    auto result = std::string();
    for (auto it = quoted.begin()+1; it != quoted.end() - 1; ++it) {
      if (*it == '\\' && (it + 1 != quoted.end() - 1))
        result += *(++it);  // skip and take next character
      else
        result += *it;
    }
    return result;
  }
};
