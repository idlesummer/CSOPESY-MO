#pragma once
#include "core/common/imports/_all.hpp"

namespace csopesy {

  /** Represents a single process instruction. */
  struct Instruction {
    using list  = vector<str>;
    using map = unordered_map<str, uint>;
    
    str opcode;
    list args;
    map meta;
  };
}
