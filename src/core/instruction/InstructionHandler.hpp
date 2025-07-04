#pragma once
#include "core/common/imports/_all.hpp"
#include "Instruction.hpp"

namespace csopesy {

  /** Represents an executable instruction handler. */
  class ProcessData; // Forward declaration
  struct InstructionHandler {
    using execute_func = function<void(const Instruction&, ProcessData&)>;
    using example_func = function<Instruction(ProcessData&)>;

    str opcode;
    execute_func execute;
    example_func example;
  };
}
