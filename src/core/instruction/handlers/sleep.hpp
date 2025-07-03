#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"

namespace csopesy::instruction {

  inline InstructionHandler make_sleep() {
    return {
      .opcode = "SLEEP",

      .execute = [](const Instruction& inst, ProcessData& proc) -> Uint {
        uint duration = cast_uint(inst.args[0]);
        proc.get_state().sleep_for(duration);
      },

      .example = [](ProcessData&) -> Instruction {
        return { "SLEEP", { "2" } };
      },
    };
  }

}
