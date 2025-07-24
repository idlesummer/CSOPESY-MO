#pragma once
#include <algorithm>  // required for all_of / any_of to work for some reason??
#include <any>
#include <cctype>
#include <cstdint>
#include <exception>
#include <functional>
#include <limits>
#include <optional>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>

using std::runtime_error;

// === Type Aliases ===
using std::any;
using std::exception;
using std::function;
using std::optional;
using byte = uint8_t;
using uint = uint32_t;
using uchar = unsigned char;
using Uint = std::optional<uint>;

// === Aliases ===
template <typename... Args> 
using func = std::function<Args...>;

template <typename Type> 
using ref = std::reference_wrapper<Type>;

template <typename Type> 
using Ref = optional<ref<Type>>;

// === Constants ===
using std::nullopt;

// === STL Utility Aliases ===
using std::ranges::all_of;
using std::ranges::any_of;
using std::ranges::count_if;
using std::ranges::find_if;
using std::ranges::none_of;
using std::any_cast;
using std::cref;
using std::erase_if;
using std::move;
using std::forward;
using std::swap;

// Iterators
template <typename Type>
auto move_iter(Type it) { 
  return std::make_move_iterator(it); 
}
