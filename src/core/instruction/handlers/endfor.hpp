#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"
#include "core/process/ProcessProgram.hpp"

namespace csopesy::instruction {
  inline InstructionHandler make_endfor() {
    return {
      .opcode = "ENDFOR",

      .execute = [](const Instruction& inst, ProcessData& proc) {
        auto& program = proc.get_program();
        auto& context = program.get_context();

        // Check for matching FOR context
        if (context.empty())
          throw runtime_error("[ENDFOR] No matching FOR context.");

        auto& frame = context.top();

        // First time hitting ENDFOR, set jump address
        if (frame.jump_addr == 0)
          context.set_jump(program.get_ip() + 1);
        
         // Decrement count and check if we're done
        if (--frame.count > 0)
          return void(program.set_ip(frame.line_addr));

        // Finished with this loop
        context.pop(); 
      },

      .example = [](ProcessData&) -> Instruction {
        return { "ENDFOR" };
      },
    };
  }
}
