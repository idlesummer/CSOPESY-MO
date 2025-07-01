#pragma once
#include "core/process/Process.hpp"
#include "core/instruction/types.hpp"

namespace csopesy::instruction {

  inline InstructionHandler make_declare() {
    return {
      .opcode = "DECLARE",

      .execute = [](Process& proc, const Instruction& inst) {
        uint value = cast_uint(inst.args[1]);
        proc.get_memory().set(inst.args[0], value);
      },

      .example = [](Process&) -> Instruction {
        return Instruction{ "DECLARE", { "x", "10" } };
      },
    };
  }
}
