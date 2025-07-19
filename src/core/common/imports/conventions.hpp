#pragma once
#include <utility> // for std::forward

#define let auto&&


template <typename Type>
constexpr decltype(auto) forward(Type&& x) noexcept {
  return std::forward<Type>(x);
}
