#include "core/common/imports/_all.hpp"


class StyleStack {
  public:

  StyleStack():
    stack(vec<pair<str,str>>()) {}  // Start with an empty list of style layers

  /** @brief Add all styles (as separate layers) to the stack. */
  void push(map<str,str>& styles) {
    for (auto& [k, v]: styles)
      stack.emplace_back(k, v);
  }

  /** @brief Remove the most recent or all layers containing a given style tag. */
  void pop(str& tag, bool all=false) {
    if (all) {
      auto matches_tag = [&](auto& layer) { return layer.first == tag; };
      erase_if(stack, matches_tag);
      return;
    }

    for (auto rit = stack.rbegin(); rit != stack.rend(); ++rit) {
      if (rit->first != tag)
        continue;

      auto it = next(rit).base();  // convert to forward iterator
      stack.erase(it);
      break;
    }
  }

  /** @brief Clear all styles from the stack. */
  void reset() { stack.clear(); }

  /** @brief Combine all active styles into one map. */
  auto get_styles() -> map<str,str> {
    auto result = map<str,str>();

    for (auto& [key, val]: stack)
      result[key] = val;
    return result;
  }

  // ------ Instance variables ------
  vec<pair<str, str>> stack;
};


class RichText {
  public:

  // ------ Class variables ------
  static inline auto TAG_PATTERN = regex(R"(\[(/?)([^\[\]]+?)\])");
  static inline auto PLACE_LB    = "\x01";
  static inline auto PLACE_RB    = "\x02";
  static inline auto ANSI_RESET  = "\033[0m";

  static inline auto STYLES = map<str,str>{
    {"bold",      "1"}, 
    {"dim",       "2"}, 
    {"italic",    "3"},
    {"underline", "4"}, 
    {"strike",    "9"},
  };

  static inline auto NAMED_COLORS = map<str,str>{
    {"black",     "#000000"}, 
    {"red",       "#ff0000"}, 
    {"green",     "#00ff00"},
    {"yellow",    "#ffff00"}, 
    {"blue",      "#0000ff"}, 
    {"magenta",   "#ff00ff"},
    {"cyan",      "#00ffff"}, 
    {"white",     "#ffffff"}, 
    {"gray",      "#808080"},
    {"lightgray", "#cccccc"}, 
    {"darkred",   "#880000"},
    {"darkgreen", "#008800"}, 
    {"darkblue",  "#000088"},
  };

  static inline auto ALIASES = map<str,str>{
    {"b", "bold"}, 
    {"i", "italic"}, 
    {"u", "underline"},
    {"s", "strike"}, 
    {"d", "dim"},
  };

  RichText(str input):
    raw         (input),        // Store the raw input string with markup tags
    text        (""s),          // Initialize rendered ANSI-formatted text (to be set by render())
    visual_size (0u),           // Number of visible characters (excluding ANSI codes)
    stack       (StyleStack())  // Initialize the style stack for nested formatting
  {
    text = render();            // Immediately render text
  }

  /** @brief Returns the rendered ANSI-formatted string. */
  auto to_str() const -> const str& {
    return text;
  }

  /** @brief Returns the number of visible characters (excluding ANSI codes). */
  auto size() const -> size_t {
    return visual_size;
  }

  /** @brief Streams the rendered text to an output stream (e.g., std::cout). */
  friend ostream& operator<<(ostream& os, const RichText& rt) {
    return os << rt.text;
  }

  // ------ Instance variables ------
  
  str raw;
  str text;
  uint visual_size;
  StyleStack stack;

  // ------ Internal helpers ------

  private:

  /** @brief Removes ANSI escape codes from the given string. */
  auto strip_ansi(str& text) -> str {
    static auto ansi_re = regex(R"(\x1B\[[0-9;]*m)");
    return regex_replace(text, ansi_re, "");
  }

  /** @brief Converts a hex color string or named color to an RGB tuple. */
  auto hex_to_rgb(str value) -> tuple<int,int,int> {
    if (NAMED_COLORS.count(value))
      value = NAMED_COLORS.at(value);

    value.erase(0, 1); // remove #
    int r = stoi(value.substr(0, 2), nullptr, 16);
    int g = stoi(value.substr(2, 2), nullptr, 16);
    int b = stoi(value.substr(4, 2), nullptr, 16);
    return { r, g, b };
  }

  /** @brief Converts a style map to an ANSI escape code string. */
  auto to_ansi(const map<str,str>& styles) -> str {
    auto codes = vec<str>();

    for (auto& [key, value]: styles) {
      if (STYLES.count(key)) {
        codes.push_back(STYLES.at(key));

      } else if (key == "fg") {
        auto [r, g, b] = hex_to_rgb(value);
        codes.push_back(format("38;2;{};{};{}", r, g, b));

      } else if (key == "bg") {
        auto [r, g, b] = hex_to_rgb(value);
        codes.push_back(format("48;2;{};{};{}", r, g, b));
      }
    }
    if (codes.empty()) return "";
    return format("\033[{}m", join(codes, ';'));
  }

  auto join(vec<str>& parts, char delim) -> str {
    if (parts.empty()) return "";

    auto oss = osstream();
    oss << parts[0]; // Add first element without delimiter

    for (auto i = 1u; i < parts.size(); ++i)
      oss << delim << parts[i];
    return oss.str();
  }


  auto parse(str& content) -> map<str,str> {
    auto result = map<str,str>();
    auto iss = isstream(content);
    auto token = ""s;

    while (iss >> token) {
      auto eq = token.find('=');
      if (eq != NPOS) {
        auto key = token.substr(0, eq);
        auto val = token.substr(eq + 1);
        key = ALIASES.count(key) ? ALIASES.at(key) : key;
        result[key] = val;

      } else {
        auto key = ALIASES.count(token) ? ALIASES.at(token) : token;
        result[key] = "true";
      }
    }
    return result;
  }

  auto render() -> str {
    auto out = ""s;
    auto text = raw;
    auto index = 0u;
    text = regex_replace(text, regex(R"(\\\[)"), PLACE_LB);
    text = regex_replace(text, regex(R"(\\\])"), PLACE_RB);

    auto match = smatch();
    auto view = str(text.begin() + index, text.end());

    while (regex_search(view, match, TAG_PATTERN)) {
      out += view.substr(0, match.position());

      auto is_closing = match[1] == "/";
      str tag_content = match[2];

      if (is_closing) {
        if (tag_content.empty()) {
          stack.reset();
          out += ANSI_RESET;
        
        } else {
          str resolved = ALIASES.count(tag_content) ? ALIASES.at(tag_content) : tag_content;
          stack.pop(resolved);
          out += ANSI_RESET + to_ansi(stack.get_styles());
        }

      } else {
        auto styles = parse(tag_content);
        stack.push(styles);
        out += to_ansi(styles);
      }

      index += match.position() + match.length();
      view = str(text.begin() + index, text.end());
    }

    out += text.substr(index);                  // Append remaining raw text
    out += ANSI_RESET;                          // Always reset at end
    replace_if(out, [](char c) { return c == *PLACE_LB; }, '[');
    replace_if(out, [](char c) { return c == *PLACE_RB; }, ']');

    visual_size = strip_ansi(out).size();       // Only count visible characters
    return out;
  }
};


#ifdef _WIN32
#include <windows.h>
#endif

void enable_ansi() {
#ifdef _WIN32
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  GetConsoleMode(hOut, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
#endif
}


int main() {
  enable_ansi();

  auto t1 = RichText("Hello [b]World[/b]");
  auto t2 = RichText("[i]Italic[/i] text");
  auto t3 = RichText("[u]Underlined[/u] content");
  auto t4 = RichText("[strike]Struck[/strike] out");
  auto t5 = RichText("Mix: [b]Bold[/] then [i]Italic[/] and [u]Underline[/]");
  auto t6 = RichText("Color test: [fg=red]Red[/fg], [fg=#00ff00]Green[/fg]");
  auto t7 = RichText("Background test: [bg=yellow]Yellow Background[/bg]");
  auto t8 = RichText("[b fg=blue]Bold Blue [fg=green]Green[/fg] Back to Blue[/b]");
  auto t9 = RichText("Escaped: \\[b\\]not bold\\[/b\\]");
  auto t10 = RichText("[b fg=red]Red Bold[/] Normal");
  auto t11 = RichText("[b]123[/]456");

  cout << "Test 1:  " << t1 << "\n";
  cout << "Test 2:  " << t2 << "\n";
  cout << "Test 3:  " << t3 << "\n";
  cout << "Test 4:  " << t4 << "\n";
  cout << "Test 5:  " << t5 << "\n";
  cout << "Test 6:  " << t6 << "\n";
  cout << "Test 7:  " << t7 << "\n";
  cout << "Test 8:  " << t8 << "\n";
  cout << "Test 9:  " << t9 << "\n";
  cout << "Test 10: " << t10 << "\n";
  cout << "Test 11: " << t11 << " (size = " << t11.size() << ")\n";
  return 0;
}
