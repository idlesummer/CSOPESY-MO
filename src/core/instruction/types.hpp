#pragma once
#include "core/common/imports/_all.hpp"

namespace csopesy {

  /** Represents a single process instruction. */
  class Process;
  struct Instruction {
    using list  = vector<str>;         ///< Argument list (e.g., variables or constants)
    using Block = vector<Instruction>; ///< Nested instructions (used in FOR loops)

    str  opcode;  ///< The instruction name (e.g., `PRINT`, `ADD`, `SLEEP`)
    list args;    ///< Positional arguments for the instruction
    Block block;  ///< Optional instruction body (only used for `FOR`)
  };

  /** Represents an executable instruction handler. */
  struct InstructionHandler {
    using execute_func = function<void(Process&, const Instruction&)>;
    using example_func = function<Instruction(Process&)>;

    str  opcode;
    execute_func execute; ///< Execution callback for the instruction
    example_func example; ///< Randomize callback for an example instruction
  };
}
