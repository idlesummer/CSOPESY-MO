#include <string>
#include <regex>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iostream>

using std::string;
using std::vector;
using std::unordered_map;
using std::ostringstream;
using std::regex;
using std::smatch;
using std::regex_search;
using std::regex_replace;
using std::cout;


class StyleStack {
  vector<unordered_map<string, string>> stack;

  public:
    StyleStack():
      stack (vector<unordered_map<string, string>>()) {}

  void push(const unordered_map<string, string>& styles) {
    for (const auto& [k, v] : styles)
      stack.push_back({ {k, v} });
  }

  void pop(const string& tag, bool all = false) {
    if (all) {
      std::erase_if(stack, [&](auto& layer) { return layer.contains(tag); });
      return;
    }

    for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
      if (it->contains(tag)) {
        stack.erase(std::next(it).base());
        break;
      }
    }
  }

  void reset() { stack.clear(); }

  auto get_styles() const -> unordered_map<string, string> {
    auto result = unordered_map<string, string>();
    
    for (auto& layer: stack)
      result.insert(layer.begin(), layer.end());
    return result;
  }
};


class RichText {
  string raw;
  string rendered;
  size_t visual_size = 0;
  StyleStack stack;

  static inline const regex TAG_PATTERN = regex(R"(\[(/?)([^\[\]]+?)\])");
  static constexpr const char* PLACE_LB = "\x01";
  static constexpr const char* PLACE_RB = "\x02";
  static constexpr const char* ANSI_RESET = "\033[0m";

  static inline const auto STYLES = unordered_map<string, string>{
    {"bold", "1"}, {"dim", "2"}, {"italic", "3"},
    {"underline", "4"}, {"strike", "9"}
  };

  static inline const auto NAMED_COLORS = unordered_map<string, string>{
    {"black", "#000000"}, {"red", "#ff0000"}, {"green", "#00ff00"},
    {"yellow", "#ffff00"}, {"blue", "#0000ff"}, {"magenta", "#ff00ff"},
    {"cyan", "#00ffff"}, {"white", "#ffffff"}, {"gray", "#808080"},
    {"lightgray", "#cccccc"}, {"darkred", "#880000"},
    {"darkgreen", "#008800"}, {"darkblue", "#000088"}
  };

  static inline const auto ALIASES = unordered_map<string, string>{
    {"b", "bold"}, {"i", "italic"}, {"u", "underline"},
    {"s", "strike"}, {"d", "dim"}
  };

public:
  RichText(const string& input):
    raw(input) 
  {
    rendered = render();
  }

  auto strip_ansi(const string& text) const -> string {
    static const std::regex ansi_re(R"(\x1B\[[0-9;]*m)");
    return std::regex_replace(text, ansi_re, "");
  }

  auto hex_to_rgb(string value) const -> std::tuple<int, int, int> {
    if (NAMED_COLORS.count(value))
      value = NAMED_COLORS.at(value);
    value.erase(0, 1); // remove #
    int r = std::stoi(value.substr(0, 2), nullptr, 16);
    int g = std::stoi(value.substr(2, 2), nullptr, 16);
    int b = std::stoi(value.substr(4, 2), nullptr, 16);
    return {r, g, b};
  }

  auto to_ansi(const unordered_map<string, string>& styles) const -> string {
    auto codes = vector<string>();
    for (const auto& [key, value] : styles) {
      if (STYLES.count(key)) {
        codes.push_back(STYLES.at(key));
      } else if (key == "fg") {
        auto [r, g, b] = hex_to_rgb(value);
        codes.push_back("38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b));
      } else if (key == "bg") {
        auto [r, g, b] = hex_to_rgb(value);
        codes.push_back("48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b));
      }
    }
    if (codes.empty()) return "";
    return "\033[" + join(codes, ";") + "m";
  }

  auto join(const vector<string>& parts, const string& delim) const -> string {
    if (parts.empty()) return "";
    string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i)
      result += delim + parts[i];
    return result;
  }

  auto parse_tag(const string& content) const -> unordered_map<string, string> {
    unordered_map<string, string> result;
    std::istringstream iss(content);
    string token;
    while (iss >> token) {
      auto eq = token.find('=');
      if (eq != string::npos) {
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

  auto render() -> string {
    string out;
    string text = raw;
    size_t index = 0;
    text = std::regex_replace(text, std::regex(R"(\\\[)"), PLACE_LB);
    text = std::regex_replace(text, std::regex(R"(\\\])"), PLACE_RB);

    smatch match;
    auto view = string(text.begin() + index, text.end());

    while (regex_search(view, match, TAG_PATTERN)) {
      out += view.substr(0, match.position());

      bool is_closing = match[1] == "/";
      string tag_content = match[2];

      if (is_closing) {
        if (tag_content.empty()) {
          stack.reset();
          out += ANSI_RESET;
        } else {
          string resolved = ALIASES.count(tag_content) ? ALIASES.at(tag_content) : tag_content;
          stack.pop(resolved);
          out += ANSI_RESET + to_ansi(stack.get_styles());
        }
      } else {
        auto styles = parse_tag(tag_content);
        stack.push(styles);
        out += to_ansi(styles);
      }

      index += match.position() + match.length();
      view = string(text.begin() + index, text.end());
    }

    out += text.substr(index);                  // Append remaining raw text
    out += ANSI_RESET;                          // Always reset at end
    std::replace(out.begin(), out.end(), *PLACE_LB, '['); // Restore [ and ]
    std::replace(out.begin(), out.end(), *PLACE_RB, ']');

    visual_size = strip_ansi(out).size();       // Only count visible characters
    return out;

  }

  auto str() const -> const string& {
    return rendered;
  }

  auto size() const -> size_t {
    return visual_size;
  }

  friend std::ostream& operator<<(std::ostream& os, const RichText& rt) {
    return os << rt.rendered;
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

  RichText t1("Hello [b]World[/b]");
  RichText t2("[i]Italic[/i] text");
  RichText t3("[u]Underlined[/u] content");
  RichText t4("[strike]Struck[/strike] out");
  RichText t5("Mix: [b]Bold[/] then [i]Italic[/] and [u]Underline[/]");
  RichText t6("Color test: [fg=red]Red[/fg], [fg=#00ff00]Green[/fg]");
  RichText t7("Background test: [bg=yellow]Yellow Background[/bg]");
  RichText t8("[b fg=blue]Bold Blue [fg=green]Green[/fg] Back to Blue[/b]");
  RichText t9("Escaped: \\[b\\]not bold\\[/b\\]");
  RichText t10("[b fg=red]Red Bold[/] Normal");
  RichText t11("[b]123[/]456");

  std::cout << "Test 1:  " << t1 << "\n";
  std::cout << "Test 2:  " << t2 << "\n";
  std::cout << "Test 3:  " << t3 << "\n";
  std::cout << "Test 4:  " << t4 << "\n";
  std::cout << "Test 5:  " << t5 << "\n";
  std::cout << "Test 6:  " << t6 << "\n";
  std::cout << "Test 7:  " << t7 << "\n";
  std::cout << "Test 8:  " << t8 << "\n";
  std::cout << "Test 9:  " << t9 << "\n";
  std::cout << "Test 10: " << t10 << "\n";
  std::cout << "Test 11: " << t11 << " (size = " << t11.size() << ")\n";
  return 0;
}
