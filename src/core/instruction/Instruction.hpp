#pragma once
#include "core/common/imports/_all.hpp"

namespace csopesy {

  /** Represents a single process instruction. */
  struct Instruction {
    using list = vector<Instruction>;
    using arg_list  = vector<str>;
   
    str opcode;     ///< Operation name, e.g., `FOR`, `PRINT`, etc.
    arg_list args;  ///< List of string arguments

  /**
   * Metadata: cached exit address for control-flow blocks, e.g., `FOR`, `IF`.
   * Set dynamically to allow skipping. A value of 0 means unset.
   */
    mutable uint exit = 0;
  };
}
