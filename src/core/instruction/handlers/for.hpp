#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"
#include "print.hpp"
#include "sleep.hpp"

namespace csopesy::instruction {
  inline InstructionHandler make_for() {
    return {
      .opcode = "FOR",

      .execute = [](const Instruction& inst, ProcessData& proc) {
        // Do nothing, control instructions are handled internally by ProcessProgram
      },

      .example = [](ProcessData& proc) -> Instruction {
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
