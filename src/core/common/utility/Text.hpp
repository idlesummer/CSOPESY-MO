#pragma once
#include "core/common/imports/_all.hpp"


/** @brief Handles foreground and background ANSI color codes. */
class TextColor {
  public:

  TextColor():
    fg (-1),   // Default: no foreground color set
    bg (-1) {} // Default: no background color set

  auto set_fg(uint code) -> TextColor& { fg = code; return *this; }
  auto set_bg(uint code) -> TextColor& { bg = code; return *this; }

  /** 
   * @brief Applies ANSI color codes to the input text.
   * @example 
   *   TextColor().set_fg(196).set_bg(15).use("Error");
   *   // → "\033[38;5;196m\033[48;5;15mError\033[0m"
   */
  auto to(const str& text) const -> str {
    auto out = ""s;
    if (fg >= 0) out += format("\033[38;5;{}m", fg);
    if (bg >= 0) out += format("\033[48;5;{}m", bg);
    return format("{}{}\033[0m", out, text);
  }

  // ------ Member variables -------
  int fg;
  int bg;
};


/** @brief Handles bold, italic, underline, reverse styles. */
class TextStyle {
  public:

  TextStyle():
    bold      (false),    // Bold style (ANSI \033[1m)
    italic    (false),    // Italic style (ANSI \033[3m)
    underline (false),    // Underline style (ANSI \033[4m)
    reverse   (false) {}  // Reverse video (ANSI \033[7m)

  auto set_bold() -> TextStyle& { return bold = true, *this; }
  auto set_italic() -> TextStyle& { return italic = true, *this; }
  auto set_underline() -> TextStyle& { return underline = true, *this; }
  auto set_reverse() -> TextStyle& { return reverse = true, *this; }

  /** 
   * @brief Returns the text with ANSI style codes applied.
   * @example
   *   TextStyle().set_bold(true).set_italic(true).to("Hello");
   *   // → "\033[1m\033[3mHello"
   */
  auto to(const str& text) const -> str {
    auto out = ""s;
    if (bold)      out += "\033[1m";
    if (italic)    out += "\033[3m";
    if (underline) out += "\033[4m";
    if (reverse)   out += "\033[7m";
    return out + text;
  }

  // ------ Member variables -------
  bool bold;
  bool italic;
  bool underline;
  bool reverse;
};


/** @brief Handles padding and alignment. */
class TextAlign {
  public:

  TextAlign():
    align ('r'),    // Alignment: 'l' = left, 'c' = center, 'r' = right
    fill  (' ') {}  // Padding fill character (default: space)

  auto set_align(char a) -> TextAlign& { return align = a, *this; }
  auto set_fill(char f) -> TextAlign& { return fill = f, *this; }

  /** 
   * @brief Applies alignment using current terminal width.
   * @example TextAlign().set_align('c').set_fill('.').to("Hello");
   */
  auto to(const str& input) const -> str {
    uint width = get_terminal_width();

    if (width <= len(input))
      return input;

    uint pad = width - len(input);
    if (align == 'l')
      return input + str(pad, fill);

    else if (align == 'c') {
      uint lpad = pad / 2;
      uint rpad = pad - lpad;
      return str(lpad, fill) + input + str(rpad, fill);
    }
    
    return str(pad, fill) + input; // default right
  }

  // ------ Member variables -------
  char align;
  char fill;
};


/** @brief Main class to represent styled, padded terminal text. */
class Text {
  public:

  Text(const str& s): 
    raw(s),     // Original input text
    color(),    // Handles foreground and background colors
    style(),    // Handles text styles (bold, italic, etc.)
    align() {}  // Handles alignment and padding

  /** @brief Enables ANSI and Unicode output (once at startup). */
  static void enable() {
    enable_unicode();
  }

  /** @brief Returns the fully formatted output string. */
  auto get() const -> str {
    auto padded = align.to(raw);
    auto styled = style.to(padded);
    return color.to(styled);
  }

  /** @brief Applies style/color/padding using a style string. */
  auto use(const str& spec) -> Text& {
    auto stream = isstream(spec);
    auto token = ""s;

    while (getline(stream, token, '+')) {
      // Text style tokens
      if (token == "b") style.set_bold();
      else if (token == "i") style.set_italic();
      else if (token == "u") style.set_underline();
      else if (token == "rev") style.set_reverse();

      // Named foreground colors
      else if (token == "r")  color.set_fg(196);
      else if (token == "gr") color.set_fg(46);
      else if (token == "yl") color.set_fg(226);
      else if (token == "bl") color.set_fg(27);
      else if (token == "mg") color.set_fg(201);
      else if (token == "cy") color.set_fg(51);
      else if (token == "wh") color.set_fg(15);

      // 256-color support
      else if (token.starts_with("fg")) color.set_fg(stoul(token.substr(2)));
      else if (token.starts_with("bg")) color.set_bg(stoul(token.substr(2)));

      else if (token == "pl") align.set_align('l');
      else if (token == "pc") align.set_align('c');
      else if (token == "pr") align.set_align('r');
      else if (token.starts_with("pf") && token.length() == 3)
        align.set_fill(token[2]);
    }

    return *this;
  }

  /** @brief Alternate way to apply DSL: Text("x")["gr+b+p10c"] */
  auto operator[](const str& spec) -> Text {
    auto copy = *this;
    return copy.use(spec);
  }

  /** @brief Enables usage with std::ostream (e.g. `cout << text`) */
  friend auto operator<<(ostream& out, const Text& t) -> ostream& {
    return out << t.get();
  }

  private:

  // ------ Member variables ------
  str raw;
  TextColor color;
  TextStyle style;
  TextAlign align;  
};
