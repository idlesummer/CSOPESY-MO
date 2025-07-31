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


/** 
 * @brief Generic cast utility. 
 * Use `cast<T>(x)` freely — auto-selects `any_cast` or `static_cast` as needed.
 */
template <typename Target, typename Source>
auto cast(Source&& x) -> Target {
  if constexpr (std::is_same_v<std::decay_t<Source>, std::any>)
    return std::any_cast<Target>(x);
  else
    return static_cast<Target>(std::forward<Source>(x));
}
