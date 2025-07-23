#pragma once
#include "core/common/imports/_all.hpp"


/** Represents a single process instruction. */
struct Instruction {
  using Script = vec<Instruction>;

  str opcode;     ///< Operation name, e.g., `FOR`, `PRINT`, etc.
  vec<str> args;      ///< List of string arguments

  /**
   * Metadata: cached exit address for control-flow blocks, e.g., `FOR`, `IF`.
   * Set dynamically to allow skipping. A value of 0 means unset.
   */
  mutable uint exit = 0;
};
