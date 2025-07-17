#pragma once
#include <any>          // for std::any, std::any_cast
#include <cstdint>      // for std::uint32_t
#include <string>       // for std::string
#include <type_traits>  // for std::decay_t
#include <utility>      // for std::forward


using std::bad_any_cast;

// Import standard string conversion functions
using std::stoi;
using std::stof;
using std::stoul;
using std::to_string;

// === Generic any cast function ===
template <typename Type, typename From>
Type cast(From&& x) {
  using Raw = std::decay_t<From>;

  if constexpr (std::is_same_v<Raw, std::any>)
    return std::any_cast<Type>(x);
  else
    return static_cast<Type>(std::forward<From>(x));
}

// === Aliases for clarity ===
template <typename From> 
inline int cast_int (From&& x) { 
  return cast<int>(std::forward<From>(x)); 
}

template <typename From> 
inline std::uint32_t cast_uint (From&& x) { 
  return cast<uint32_t>(std::forward<From>(x)); 
}

template <typename From> 
inline std::string cast_str (From&& x) {
  return cast<std::string>(std::forward<From>(x)); 
}
