#pragma once
#include "core/common/imports/_all.hpp"
#include "core/common/utility/Text.hpp"

/** @brief Renders and styles the shell banner using the Text system. */
class ShellBanner {
  public:

  /** @brief Renders the styled banner as a string. */
  static auto get() -> str {
    auto out = osstream();
    for (auto& line: lines())
      out << Text(line)["gr+pc"] << '\n';
    return out.str();
  }

  // ------ Internal helpers ------

  private:
  
  /** @brief The raw banner ASCII lines. */
  static auto lines() -> const vec<str>& {
    static const auto banner_lines = vec<str>{
      "",
      "  ██╗  ██████╗ ███████╗  ██████╗  ██████╗  ███████╗ ███████╗ ██╗   ██╗ ██╗  ",
      " ██╔╝ ██╔════╝ ██╔════╝ ██╔═══██╗ ██╔══██╗ ██╔════╝ ██╔════╝ ╚██╗ ██╔╝ ╚██╗ ",
      "██╔╝  ██║      ███████╗ ██║   ██║ ██████╔╝ █████╗   ███████╗  ╚████╔╝   ╚██╗",
      "╚██╗  ██║      ╚════██║ ██║   ██║ ██╔═══╝  ██╔══╝   ╚════██║   ╚██╔╝    ██╔╝",
      " ╚██╗ ╚██████╗ ███████║ ╚██████╔╝ ██║      ███████╗ ███████║    ██║    ██╔╝ ",
      "  ╚═╝  ╚═════╝ ╚══════╝  ╚═════╝  ╚═╝      ╚══════╝ ╚══════╝    ╚═╝    ╚═╝  ",
      "",
      "Dicayanan | Maramag | Maunahan | Villaver",
    };
    return banner_lines;
  }
};
