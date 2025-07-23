#pragma once
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>
#include <stdexcept>
#include "casts.hpp"
#include "containers.hpp"
#include "misc.hpp"


/** Random utility class */
class Rand {
  using mt19937   = std::mt19937;
  using int_dist  = std::uniform_int_distribution<>;
  using uint_dist = std::uniform_int_distribution<unsigned>;
  using real_dist = std::uniform_real_distribution<float>;

  inline static mt19937 rng = mt19937(std::random_device{}());

public:

  /** Random signed integer in [min, max] */
  static int num(int min, int max) {
    auto dist = int_dist(min, max);
    return dist(rng);
  }

  /** Random unsigned integer in [min, max] */
  static auto num(uint min, uint max) -> uint {
    auto dist = uint_dist(min, max);
    return dist(rng);
  }

  /** Returns true with probability 1/odds */
  static auto chance(uint odds) -> bool {
    return odds != 0 && num(1u, odds) == 1u;
  }

  /** Random float in [min, max) */
  static auto real(float min, float max) -> float {
    auto dist = real_dist(min, max);
    return dist(rng);
  }

  /** Pick a Random element from a non-empty vector */
  template <typename Type>
  static auto pick(Type&& list) -> decltype(auto) {
    if (list.empty())
      throw runtime_error("Cannot pick from empty list.");
    return std::forward<Type>(list)[num(0, static_cast<int>(list.size()) - 1)];
  }

  /** Shuffle a vector in place */
  template <typename Type>
  static auto shuffle(std::vector<Type>& list) -> std::vector<Type>& {
    std::shuffle(list.begin(), list.end(), rng);
    return list;
  }
};
