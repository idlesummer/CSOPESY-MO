#pragma once
#include "core/common/imports/_all.hpp"


class StyleStack {
  public:

  StyleStack():
    stack(vec<pair<str,str>>()) {}  // Start with an empty list of style layers

  /** @brief Add all styles (as separate layers) to the stack. */
  void push(umap<str,str> styles) {
    for (auto& [key, val]: styles)
      stack.emplace_back(move(key), move(val));
  }

  /** @brief Remove the most recent layer containing a given style tag. */
  void pop(str tag) {
    for (auto rit = stack.rbegin(); rit != stack.rend(); ++rit) {
      if (rit->first != tag)
        continue;

      auto it = next(rit).base();  // convert to forward iterator
      stack.erase(it);
      break;
    }
  }

  /** @brief Clear all styles from the stack. */
  void reset() { 
    stack.clear(); 
    stack.shrink_to_fit();
  }

  /** @brief Combine all active styles into one umap. */
  auto get_styles() -> umap<str,str> {
    auto result = umap<str,str>();

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
  static inline auto TAG_PATTERN = regex(R"(\[(/?)([^\[\]]*?)\])");
  static inline auto ESC_LB      = regex(R"(\\\[)");
  static inline auto ESC_RB      = regex(R"(\\\])");
  static inline auto PLACE_LB    = "\x01";
  static inline auto PLACE_RB    = "\x02";
  static inline auto ANSI_RESET  = "\033[0m";

  static inline auto STYLES = umap<str,str>{
    {"bold",      "1"}, 
    {"dim",       "2"}, 
    {"italic",    "3"},
    {"underline", "4"}, 
    {"strike",    "9"},
  };

  static inline auto COLORS = umap<str,str>{
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

  static inline auto ALIASES = umap<str,str>{
    {"b", "bold"}, 
    {"i", "italic"}, 
    {"u", "underline"},
    {"s", "strike"}, 
    {"d", "dim"},
  };

  RichText(str input):
    raw   (move(input)),  // Store the raw input string with markup tags
    text  (""s),          // Initialize rendered ANSI-formatted text (to be set by render())
    size  (0u),           // Number of visible characters (excluding ANSI codes)
    stack (StyleStack())  // Initialize the style stack for nested formatting
  {
    text = render();                // Immediately render text
    size = strip_ansi(text).size(); // Only count visible characters
  }
  
  /** @brief Enables ANSI and Unicode output (once at startup). */
  static void enable() {
    initialize_terminal();
  }

  /** @brief Streams the rendered text to an output stream (e.g., std::cout). */
  friend ostream& operator<<(ostream& os, const RichText& rt) {
    return os << rt.text;
  }

  // ------ Instance variables ------
  
  str raw;
  str text;
  uint size;
  StyleStack stack;

  // ------ Internal helpers ------

  private:

  /** @brief Removes ANSI escape codes from the given string. */
  auto strip_ansi(str& text) -> str {
    static auto ansi_re = regex(R"(\x1B\[[0-9;]*m)");
    return regex_replace(text, ansi_re, "");
  }

  /** @brief Converts a hex color string or named color to an RGB tuple. */
  auto hex_to_rgb(str value) -> tuple<uint,uint,uint> {
    value.erase(0, 1); // remove leading #
    auto r = stoul(value.substr(0, 2), nullptr, 16);
    auto g = stoul(value.substr(2, 2), nullptr, 16);
    auto b = stoul(value.substr(4, 2), nullptr, 16);
    return { r, g, b };
  }

  auto is_valid_hex_color(const str& value) -> bool {
    return value.size() == 7
      && value[0] == '#'
      && all_of(value.begin()+1, value.end(), ::isxdigit);
  }

  /** @brief Converts a style umap to an ANSI escape code string. */
  auto to_ansi(const umap<str,str>& styles) -> str {
    auto codes = vec<str>();

    for (auto& [key, value]: styles) {
      if (STYLES.contains(key)) {
        codes.push_back(STYLES.at(key));

      } else if (key == "fg") {
        auto hex = COLORS.contains(value) ? COLORS.at(value) : value;
        if (!is_valid_hex_color(hex)) continue; // skip if not valid hex color

        auto [r, g, b] = hex_to_rgb(hex);
        codes.push_back(format("38;2;{};{};{}", r, g, b));

      } else if (key == "bg") {
        auto hex = COLORS.contains(value) ? COLORS.at(value) : value;
        if (!is_valid_hex_color(hex)) continue; // skip if not valid hex color
        
        auto [r, g, b] = hex_to_rgb(hex);
        codes.push_back(format("48;2;{};{};{}", r, g, b));
      }
    }

    return codes.empty() ? "" : format("\033[{}m", join(codes, ';'));
  }

  auto resolve_tag(str& tag) -> str {
    return ALIASES.contains(tag) ? ALIASES.at(tag) : tag;
  }

  auto parse_to_styles(str& tag) -> umap<str,str> {
    auto result = umap<str,str>();
    auto stream = isstream(tag);
    auto token = ""s;

    while (stream >> token) {
      auto eq = token.find('=');
      
      if (eq == npos) {
        auto key = resolve_tag(token);
        result[key] = "";   // Flag tag with no value ([bold] becomes {"bold": "-"})
        continue;
      }

      auto key = token.substr(0, eq);
      auto val = token.substr(eq+1);
      key = resolve_tag(key);
      result[key] = val;
    }

    return result;
  }

  auto render() -> str {
    auto out = ""s;
    auto index = 0u;

    // Escape literal brackets (\[ and \]) so they are not parsed as tags
    auto text = regex_replace(raw, ESC_LB, PLACE_LB);
    text = regex_replace(text, ESC_RB, PLACE_RB);

    auto match = smatch();
    while (regex_search(text.cbegin() + index, text.cend(), match, TAG_PATTERN)) {
      out += text.substr(index, match.position());  // Add all plain text before the tag

      auto is_closing = match[1].str() == "/";
      auto tag = trim(match[2].str());  // raw tag content

      if (is_closing) {           // Handle [/], which resets all styles
        if (tag.empty()) {                          
          stack.reset();
          out += ANSI_RESET;
        
        } else {                  // Handle [/<tag>] - pop specific style
          stack.pop(resolve_tag(tag));
          out += ANSI_RESET + to_ansi(stack.get_styles());
        }

      } else {                    // Handle [tag ...] — parse and push new style(s)
        stack.push(parse_to_styles(tag));
        out += to_ansi(stack.get_styles());
      }

      index += match.position() + match.length();  // Move index forward to just after the matched tag
    }

    // Append remaining raw text and unescape any \[ or \]
    out += text.substr(index);
    replace(out.begin(), out.end(), *PLACE_LB, '[');
    replace(out.begin(), out.end(), *PLACE_RB, ']');


    // Only append reset if styling was ever applied
    if (!stack.get_styles().empty())
      out += ANSI_RESET;

    stack.reset();  // To free memory once text has been rendered
    return out;
  }
};


// int main() {
//   auto t1 = RichText("Hello [b]World[/b]");
//   auto t2 = RichText("[i]Italic[/i] text");
//   auto t3 = RichText("[u]Underlined[/u] content");
//   auto t4 = RichText("[strike]Struck[/strike] out");
//   auto t5 = RichText("Mix: [b]Bold[/] then [i]Italic[/] and [u]Underline[/]");
//   auto t6 = RichText("Color test: [fg=red]Red[/fg], [fg=#00ff00]Green[/fg]");
//   auto t7 = RichText("Background test: [bg=yellow]Yellow Background[/bg]");
//   auto t8 = RichText("[b fg=blue]Bold Blue [fg=green]Green[/fg] Back to Blue[/b]");
//   auto t9 = RichText("Escaped: \\[b\\]not bold\\[/b\\]");
//   auto t10 = RichText("[b fg=red]Red Bold[/] Normal");
//   auto t11 = RichText("[b]123[/]456");

//   cout << "Test 1:  " << t1 << "\n";
//   cout << "Test 2:  " << t2 << "\n";
//   cout << "Test 3:  " << t3 << "\n";
//   cout << "Test 4:  " << t4 << "\n";
//   cout << "Test 5:  " << t5 << "\n";
//   cout << "Test 6:  " << t6 << "\n";
//   cout << "Test 7:  " << t7 << "\n";
//   cout << "Test 8:  " << t8 << "\n";
//   cout << "Test 9:  " << t9 << "\n";
//   cout << "Test 10: " << t10 << "\n";
//   cout << "Test 11: " << t11 << " (size = " << t11.size << ")\n";
//   return 0;
// }
