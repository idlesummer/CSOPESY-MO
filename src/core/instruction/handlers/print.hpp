#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionSchema.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"

namespace csopesy::instruction {

  inline InstructionHandler make_print() {
    using Schema = InstructionSchema;

    return {
      .opcode = "PRINT",
      .schema = { Schema::String() },
      .execute = [](const Instruction& inst, ProcessData& proc) {
        proc.log(inst.args[0]);
      },
    };
  }
}
