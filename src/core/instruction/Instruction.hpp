#pragma once
#include "core/common/imports/_all.hpp"

namespace csopesy {

  /** Represents a single process instruction. */
  struct Instruction {
    using list  = vector<str>;
    
    str opcode;
    list args;
  };
}
