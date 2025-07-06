#pragma once
#include "core/common/imports/_all.hpp"
#include "Instruction.hpp"

namespace csopesy {
  class ProcessData; // Forward declaration

  /** Represents an executable instruction handler. */
  struct InstructionHandler {
    using execute_func = function<void(const Instruction&, ProcessData&)>;
    using example_func = function<Instruction(ProcessData&)>;

    str opcode;           /// The opcode string representing the instruction.
    execute_func execute; ///< Function to execute the instruction
    example_func example; ///< Function to generate an example instance

    // === Metadata ===
    str end_opcode = "";  ///< For control instruction with a matching end

    // === Helpers ===

    /** Returns true if this instruction begins a control block (e.g., FOR). */
    bool is_control() const { return !end_opcode.empty(); }
  };
}
