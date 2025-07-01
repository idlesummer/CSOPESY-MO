#pragma once
#include <algorithm>  // required for all_of / any_of to work for some reason??
#include <any>
#include <cctype>
#include <cstdint>
#include <functional>
#include <limits>
#include <optional>
#include <ranges>
#include <string>
#include <utility>

namespace csopesy {

  // === Type Aliases ===
  using std::any;
  using std::function;
  using std::optional;
  using byte = uint8_t;
  using uint = uint32_t;
  using uchar = unsigned char;

  // === Reference Aliases ===
  using std::cref;
  template <typename Type> using ref = std::reference_wrapper<Type>;
  template <typename Type> using Ref = optional<ref<Type>>;

  // === Constants ===
  using std::nullopt;
  inline constexpr uint MAX = std::numeric_limits<uint>::max();

  // === STL Utility Aliases ===
  using std::ranges::all_of;
  using std::ranges::any_of;
  using std::ranges::none_of;
  using std::any_cast;
  using std::erase_if;
  using std::move;
  using std::forward;
  using std::swap;

  // Iterators
  template <typename Type>
  inline auto move_iter(Type it) { 
    return std::make_move_iterator(it); 
  }
}
