#pragma once
#include <string>     // for std::string
#include <any>        // for std::any, std::any_cast
#include <cstdlib>    // for std::strtoul
#include <cstdint>    // for uint32_t
#include <utility>    // for std::forward

namespace csopesy {

  // === Type-to-Type Casting ===
  template <typename Type>
  inline float cast_float(Type&& x) {
    return static_cast<float>(std::forward<Type>(x));
  }

  template <typename Type>
  inline int cast_int(Type&& x) {
    return static_cast<int>(std::forward<Type>(x));
  }

  // === String Conversion ===
  inline uint32_t cast_uint(const std::string& s) {
    return static_cast<uint32_t>(std::stoul(s));
  }

  // === std::any Conversion ===
  inline int to_int (const std::any& val)    { 
    return std::any_cast<int>(val); 
  }

  inline std::string to_str(const std::any& val) { 
    return std::any_cast<std::string>(val); 
  }

  inline uint32_t to_uint(const std::any& val) { 
    return std::any_cast<uint32_t>(val); 
  }
}
