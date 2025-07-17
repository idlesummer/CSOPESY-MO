#pragma once
#include <any>          // for std::any, std::any_cast
#include <cstdint>      // for std::uint32_t
#include <string>       // for std::string
#include <type_traits>  // for std::decay_t
#include <utility>      // for std::forward


// Import error exceptions
using std::bad_any_cast;

// Import standard string conversion functions
using std::stoi;
using std::stof;
using std::stoul;
using std::to_string;

// === Generic any cast function ===
template <typename Type, typename From>
auto cast(From&& x) -> Type {
  using Raw = std::decay_t<From>;

  if constexpr (std::is_same_v<Raw, std::any>)
    return std::any_cast<Type>(x);
  else
    return static_cast<Type>(std::forward<From>(x));
}

// === Aliases for clarity ===
template <typename From> 
auto cast_int(From&& x) -> int { 
  return cast<int>(std::forward<From>(x)); 
}

template <typename From> 
auto cast_uint(From&& x) -> std::uint32_t { 
  return cast<uint32_t>(std::forward<From>(x)); 
}

template <typename From> 
auto cast_str(From&& x) -> std::string {
  return cast<std::string>(std::forward<From>(x)); 
}
