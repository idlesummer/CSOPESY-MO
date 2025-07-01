#pragma once
#include "core/process/Process.hpp"
#include "core/instruction/types.hpp"

namespace csopesy::instruction {

  inline InstructionHandler make_print() {
    return {
      .opcode = "PRINT",

      .execute = [](Process& proc, const Instruction& inst) {
        proc.log(inst.args[0]);
      },

      .example = [](Process& proc) -> Instruction {
        str message = format("Hello world from {}!", proc.get_name());
        return Instruction{ "PRINT", { move(message) }};
      },
    };
  }
}
