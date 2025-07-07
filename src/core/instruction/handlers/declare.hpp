#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"

namespace csopesy::instruction {

  inline InstructionHandler make_declare() {
    return {
      .opcode = "DECLARE",
      .execute = [](const Instruction& inst, ProcessData& proc) {
        uint value = stoul(inst.args[1]);
        proc.get_memory().set(inst.args[0], value);
      },
    };
  }
}
