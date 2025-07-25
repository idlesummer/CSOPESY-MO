#pragma once
#include "core/common/imports/_all.hpp"
#include "core/common/utility/Text.hpp"

/** @brief Renders and styles the shell banner using a vertical gradient. */
class ShellBanner {
  public:

  /** @brief Returns the full banner with vertical gradient applied. */
  auto get() -> str {
    auto mint = vec<int>{201, 254, 221};
    auto coral = vec<int>{249, 121, 121};
    auto gradient = make_gradient(coral, mint, lines.size());
    
    auto out = osstream();
    out << "\n\n";
    for (uint i = 0; i < lines.size(); ++i) {
      int code = rgb_to_ansi256(gradient[i]);
      out << Text(lines[i])[format("fg{}+pc", code)] << '\n';
    }

    out << '\n';
    out << Text(names)[format("pc+fg{}", rgb_to_ansi256(mint))] << "\n\n";
    return out.str();
  }

  private:

  // ------ Internal helpers ------

  static inline const auto names = "Dicayanan | Maramag | Maunahan | Villaver"s;
  static inline const auto lines = vec<str>{
    "  ██╗  ██████╗ ███████╗  ██████╗  ██████╗  ███████╗ ███████╗ ██╗   ██╗ ██╗  ",
    " ██╔╝ ██╔════╝ ██╔════╝ ██╔═══██╗ ██╔══██╗ ██╔════╝ ██╔════╝ ╚██╗ ██╔╝ ╚██╗ ",
    "██╔╝  ██║      ███████╗ ██║   ██║ ██████╔╝ █████╗   ███████╗  ╚████╔╝   ╚██╗",
    "╚██╗  ██║      ╚════██║ ██║   ██║ ██╔═══╝  ██╔══╝   ╚════██║   ╚██╔╝    ██╔╝",
    " ╚██╗ ╚██████╗ ███████║ ╚██████╔╝ ██║      ███████╗ ███████║    ██║    ██╔╝ ",
    "  ╚═╝  ╚═════╝ ╚══════╝  ╚═════╝  ╚═╝      ╚══════╝ ╚══════╝    ╚═╝    ╚═╝  ",
  };

  /** @brief Computes N evenly spaced vec<int> values between two endpoints. */
  auto make_gradient(vec<int>& from, vec<int>& to, uint steps) -> vec<vec<int>> {
    auto result = vec<vec<int>>();
    result.reserve(steps);

    for (uint i = 0; i < steps; ++i) {
      float t = (steps == 1) ? 0.f : float(i) / (steps - 1);
      int r = int(from[0] + (to[0] - from[0]) * t);
      int g = int(from[1] + (to[1] - from[1]) * t);
      int b = int(from[2] + (to[2] - from[2]) * t);
      result.push_back({r, g, b});
    }

    return result;
  }

  /** @brief Converts an vec<int> triplet to an ANSI 256-color code. */
  auto rgb_to_ansi256(vec<int>& rgb) -> int {
    int r = int(rgb[0] * 5 / 255.0f);
    int g = int(rgb[1] * 5 / 255.0f);
    int b = int(rgb[2] * 5 / 255.0f);
    return 16 + 36 * r + 6 * g + b;
  }
};
