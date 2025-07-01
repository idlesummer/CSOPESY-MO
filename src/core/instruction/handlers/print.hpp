#pragma once
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"

namespace csopesy::instruction {

  inline InstructionHandler make_print() {
    return {
      .opcode = "PRINT",

      .execute = [](const Instruction& inst, ProcessData& proc) {
        proc.log(inst.args[0]);
      },

      .example = [](ProcessData& proc) -> Instruction {
        str message = format("Hello world from {}!", proc.get_name());
        return Instruction{ "PRINT", { move(message) }};
      },
    };
  }
}
