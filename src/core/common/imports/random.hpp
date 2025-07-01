#pragma once
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>
#include <stdexcept>
#include "casts.hpp"
#include "containers.hpp"
#include "misc.hpp"

namespace csopesy {

  /** Random utility class */
  class Random {
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
    static uint num(uint min, uint max) {
      auto dist = uint_dist(min, max);
      return dist(rng);
    }

    /** Random float in [min, max) */
    static float real(float min, float max) {
      auto dist = real_dist(min, max);
      return dist(rng);
    }

    /** Pick a random element from a non-empty vector */
    template <typename Type>
    static Type pick(const std::vector<Type>& list) {
      if (list.empty()) throw std::runtime_error("Cannot pick from empty list.");
      return list[num(0, static_cast<int>(list.size()) - 1)];
    }

    /** Shuffle a vector in place */
    template <typename Type>
    static std::vector<Type>& shuffle(std::vector<Type>& list) {
      std::shuffle(list.begin(), list.end(), rng);
      return list;
    }
  };

}
