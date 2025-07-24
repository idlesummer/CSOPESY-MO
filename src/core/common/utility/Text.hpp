#pragma once
#include "core/common/imports/_all.hpp"


/**
 * @brief Utility class for styling terminal text using ANSI escape sequences.
 * 
 * Supports method chaining (e.g., `.bold().red().bg(15)`) and string-based
 * DSL parsing via `.use("gr+b")`. ANSI color and Unicode output are enabled
 * through the static `Text::enable()` initializer.
 * 
 * Example usage:
 *   Text::enable(); // Call once during app startup
 *   cout << "Success"_txt.use("gr+b") << '\n';
 *   cout << "Warning"_txt["yl+i"] << '\n';
 */
class Text {
  public:

  Text(str s): 
    text(move(s)),        // The raw user text to display (e.g., "Hello")
    prefix(""s),          // ANSI styling prefix (e.g., "\033[31m\033[1m" for red + bold)
    suffix("\033[0m"s) {} // ANSI reset code to clear styling after the text

  
  /** @brief Enables ANSI escape sequences and Unicode output. */
  static void enable() {
    #if defined(_WIN32)
      auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
      auto mode = DWORD(0);
      if (GetConsoleMode(handle, &mode)) {
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(handle, mode);
      }
    #endif

    enable_unicode(); // Defined in platforms.hpp
  }

  /**
   * @brief Applies multiple styles using a mini-DSL string.
   * 
   * Tokens are delimited by '+'. Each token represents a style 
   * (bold, underline) or color (red, bg15, etc.). Tokens can appear
   * in any order.
   * 
   * Supported style tokens:
   *   - b     → bold
   *   - i     → italic
   *   - u     → underline
   *   - rev   → reverse video
   * 
   * Supported color tokens:
   *   - r, gr, yl, bl, mg, cy, wh → standard named colors
   *   - fgN  → 256-color foreground (e.g., fg123)
   *   - bgN  → 256-color background (e.g., bg15)
   * 
   * Example:
   *   Text("Error").use("r+b+bg15");
   *   "Done"_txt.use("gr+b");
   */
  auto use(const str& spec) -> Text& {
    auto parts = isstream(spec);
    auto token = ""s;

    while (getline(parts, token, '+')) {
      if (token == "b") bold();
      else if (token == "i") italic();
      else if (token == "u") underline();
      else if (token == "rev") reverse();

      else if (token == "r")  red();
      else if (token == "gr") green();
      else if (token == "yl") yellow();
      else if (token == "bl") blue();
      else if (token == "mg") magenta();
      else if (token == "cy") cyan();
      else if (token == "wh") white();

      else if (token.starts_with("fg")) fg(stoul(token.substr(2)));
      else if (token.starts_with("bg")) bg(stoul(token.substr(2)));
    }

    return *this;
  }

  /** @brief Returns the fully rendered string with ANSI styling. */
  auto get() const -> str {
    return prefix + text + suffix;
  }

  // === Chainable styles ===
  auto bold()      -> Text& { prefix += "\033[1m"; return *this; }
  auto italic()    -> Text& { prefix += "\033[3m"; return *this; }
  auto underline() -> Text& { prefix += "\033[4m"; return *this; }
  auto reverse()   -> Text& { prefix += "\033[7m"; return *this; }

  auto fg(uint code) -> Text& {
    prefix += format("\033[38;5;{}m", code);
    return *this;
  }

  auto bg(uint code) -> Text& {
    prefix += format("\033[48;5;{}m", code);
    return *this;
  }

  // === Named colors (foreground)
  auto red()     -> Text& { return fg(196); }  // bright red in 256-color
  auto green()   -> Text& { return fg(46); }   // bright green
  auto yellow()  -> Text& { return fg(226); }  // bright yellow
  auto blue()    -> Text& { return fg(27); }   // bright blue
  auto magenta() -> Text& { return fg(201); }  // bright magenta
  auto cyan()    -> Text& { return fg(51); }   // bright cyan
  auto white()   -> Text& { return fg(15); }   // bright white

  // === Bracket operator for cleaner syntax: "text"_txt["r+b"]
  auto operator[](const str& spec) const -> Text {
    auto copy = *this;
    return copy.use(spec);
  }

  // === Output stream support
  friend auto operator<<(ostream& out, const Text& t) -> ostream& {
    return out << t.get();
  }

  // ------ Member variables ------
  str text;
  str prefix;
  str suffix;
};


// === Text literals ===

// Base literal
auto operator"" _txt(const char* s, size_t) -> Text { return Text(s); }

// === Color-coded literals (useful for quick stylings) ===
auto operator"" _r(const char* s, size_t)   -> Text { return Text(s).red(); }
auto operator"" _gr(const char* s, size_t)  -> Text { return Text(s).green(); }
auto operator"" _yl(const char* s, size_t)  -> Text { return Text(s).yellow(); }
auto operator"" _bl(const char* s, size_t)  -> Text { return Text(s).blue(); }
auto operator"" _mg(const char* s, size_t)  -> Text { return Text(s).magenta(); }
auto operator"" _cy(const char* s, size_t)  -> Text { return Text(s).cyan(); }
auto operator"" _wh(const char* s, size_t)  -> Text { return Text(s).white(); }
