#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"
#include "print.hpp"
#include "sleep.hpp"

namespace csopesy::instruction {
  inline InstructionHandler make_for() {
    return {
      .opcode = "FOR",

      .execute = [](Process& proc, const Instruction& inst) {
        // Do nothing, control instructions are handled internally by ProcessProgram
      },

      .example = [](Process& proc) -> Instruction {
        return Instruction {
          .opcode = "FOR", 
          .args  = { "3" },
          .block = {
            make_print().example(proc),
            make_sleep().example(proc),
          },
        };
      },
    };
  }
}
