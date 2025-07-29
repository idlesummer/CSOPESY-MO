#pragma once
#include "core/common/imports/_all.hpp"
#include "core/common/utility/Text.hpp"


/** @brief Simple ASCII-style table renderer with box borders and left-aligned rows. */
class Table {
  public:

  /** @brief Constructs a table with a specified total width. */
  Table(uint width): 
    width (width),        // Total width of the table including borders
    rows  (vec<str>()) {} // Accumulated rendered rows

  /** @brief Adds the top border using Unicode rounded corners. */
  auto head() -> Table& {
    auto line = Text("─", width - 2).get();
    rows.push_back(format("╭{}╮", line));
    return *this;
  }

  /** @brief Adds a middle horizontal bar with T-shaped corners. */
  auto bar() -> Table& {
    auto line = Text("─", width - 2).get();
    rows.push_back(format("├{}┤", line));
    return *this;
  }

  /** @brief Adds the bottom border using Unicode rounded corners. */
  auto dbar() -> Table& {
    auto line = Text("─", width - 2).get();
    rows.push_back(format("╰{}╯", line));
    return *this;
  }

  /** @brief Adds a left-aligned row of content, padded and clamped to fit. */
  auto row(const str& content) -> Table& {
    auto visual = len(content);
    auto padded = visual >= width - 4
      ? content.substr(0, width - 4)
      : content + str(width - 4 - visual, ' ');

    rows.push_back(format("│ {} │", padded));
    return *this;
  }

  /** @brief Inserts one or more blank rows (default: 1 line). */
  auto gap(uint count = 1) -> Table& {
    auto empty = str(width - 4, ' ');
    for (uint i = 0; i < count; ++i)
      rows.push_back(format("│ {} │", empty));
    return *this;
  }

  /** @brief Joins all rows into a single string. */
  auto get() const -> str {
    auto out = osstream();
    for (auto& r : rows) 
      out << r << '\n';
    return out.str();
  }

  /** @brief Prints the table using `cout << table`. */
  friend auto operator<<(ostream& out, const Table& t) -> ostream& {
    return out << t.get();
  }

  private:

  // ------ Instance variables ------
  
  uint width;       
  vec<str> rows;    

  // ------ Internal helpers ------
  /** @brief Returns visual length of a string, excluding ANSI escape sequences. */
  auto len(const str& s) -> uint {
    auto visual = 0u;
    auto in_ansi = false;

    for (char c: s) {
      if (c == '\033') { in_ansi = true; continue; }
      if (in_ansi) {
        if (c == 'm') in_ansi = false;
        continue;
      }
      ++visual;
    }

    return visual;
  }
};
