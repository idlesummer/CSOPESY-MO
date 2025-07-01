#pragma once
#include "core/process/Process.hpp"
#include "core/instruction/types.hpp"

namespace csopesy::instruction {

  inline InstructionHandler make_sleep() {
    return {
      .opcode = "SLEEP",

      .execute = [](Process& proc, const Instruction& inst) {
        uint duration = cast_uint(inst.args[0]);
        proc.get_state().sleep_for(duration);
      },

      .example = [](Process&) -> Instruction {
        return Instruction{ "SLEEP", { "2" } };
      },
    };
  }

}
