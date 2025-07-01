#pragma once
#include "Instruction.hpp"
#include "core/process/ProcessData.hpp"

namespace csopesy {

  /** Represents an executable instruction handler. */
  struct InstructionHandler {
    using execute_func = function<void(const Instruction&, ProcessData&)>;
    using example_func = function<Instruction(ProcessData&)>;

    str opcode;
    execute_func execute;
    example_func example;
  };

}
