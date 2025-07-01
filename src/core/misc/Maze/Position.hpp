#pragma once
#include "core/common/imports/_all.hpp"

namespace csopesy::Position {  
  
  // Encode two integers (x, y) into a single integer
  inline int pos(int x, int y) {
    return (x << 16) | (y & 0xFFFF);
  }

  // Decode a single encoded integer back into a position (x, y)
  inline pair<int, int> pos(int encoded) {
    int x = encoded >> 16;
    int y = encoded & 0xFFFF;
    
    // If y is negative, sign-extend it properly
    return { x, (y & 0x8000 ) ? (y | 0xFFFF0000) : y };
  }
}
