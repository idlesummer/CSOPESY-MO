#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"

namespace csopesy::instruction {
  inline InstructionHandler make_add() {
    return {
      .opcode = "ADD",

      .execute = [](const Instruction& inst, ProcessData& proc) -> Uint {
        uint lhs = proc.get_memory().get(inst.args[1]);
        uint rhs = proc.get_memory().resolve(inst.args[2]);
        proc.get_memory().set(inst.args[0], lhs + rhs);
      },

      .example = [](ProcessData&) -> Instruction {
        return { "ADD", { "z", "x", "y" }};
      },
    };
  }

}
