#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"

namespace csopesy::instruction {
  inline InstructionHandler make_for() {
    return {
      .opcode = "FOR",

      .execute = [](const Instruction& inst, ProcessData& proc) {
        auto& program = proc.get_program();
        auto& context = program.get_context();
        
        uint line_addr = program.get_ip();
        uint count = cast_uint(inst.args[0]);
        
        // Only push if no frame exists or the top isn't for this same line
        if (context.empty() || context.top().line_addr != line_addr)
          context.push("FOR", line_addr, 0, count);
      },

      .example = [](ProcessData& proc) -> Instruction {
        return { "FOR", { "3" }};
      },
    };
  }
}
