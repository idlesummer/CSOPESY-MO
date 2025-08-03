#pragma once
#include <algorithm>  // required for all_of / any_of to work for some reason??
#include <any>
#include <cctype>
#include <cstdint>
#include <exception>
#include <functional>
#include <limits>
#include <optional>
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
using uint64 = uint64_t;
using uchar = unsigned char;
using Uint = std::optional<uint>;

// === Aliases ===
template <typename Type>
using opt = optional<Type>;

template <typename... Args> 
using func = std::function<Args...>;

template <typename Type> 
using ref = std::reference_wrapper<Type>;

// === Constants ===
using std::nullopt;

// === STL Utility Aliases ===
using std::any_cast;
using std::cref;
using std::erase_if;
using std::move;
using std::forward;
using std::swap;
