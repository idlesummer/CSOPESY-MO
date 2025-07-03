#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"
#include "core/process/ProcessProgram.hpp"

namespace csopesy::instruction {
  inline InstructionHandler make_end() {
    return {
      .opcode = "END",

      .execute = [](const Instruction& inst, ProcessData& proc) {
        auto& prog = proc.get_program();
        auto& ctx  = prog.get_context();

        if (ctx.empty()) {
          proc.log("[END] Error: context stack is empty.");
          return;
        }

        auto& top = ctx.back();
        if (--top.remaining > 0) {
          prog.set_ip(top.begin_ip + 1);  // jump to first instruction inside FOR
        } else {
          ctx.pop_back();  // done with the loop
        }
      },

      .example = [](ProcessData&) -> Instruction {
        return Instruction{ "END", {} };
      },
    };
  }
}
