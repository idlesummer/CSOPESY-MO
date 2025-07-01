#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h> // Doesn't define min/max since NOMINMAX is set
#include <algorithm> // Redefines std::min/std::max

namespace csopesy {
  using std::min;
  using std::max;
}
