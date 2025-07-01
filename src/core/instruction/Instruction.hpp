#pragma once
#include "core/common/imports/_all.hpp"

namespace csopesy {

  /** Represents a single process instruction. */
  struct Instruction {
    using list  = vector<str>;
    using Block = vector<Instruction>; // used for FOR

    str opcode;
    list args;
    Block block;
  };
}
