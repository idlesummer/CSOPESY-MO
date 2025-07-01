#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"
#include "core/instruction/types.hpp"

namespace csopesy::instruction {
  inline InstructionHandler make_add() {
    return {
      .opcode = "ADD",

      .execute = [](Process& proc, const Instruction& inst) {
        uint lhs = proc.get_memory().get(inst.args[1]);
        uint rhs = proc.get_memory().resolve(inst.args[2]);
        proc.get_memory().set(inst.args[0], lhs + rhs);
      },

      .example = [](Process&) -> Instruction {
        return Instruction{ "ADD", { "z", "x", "y" }};
      },
    };
  }

}
