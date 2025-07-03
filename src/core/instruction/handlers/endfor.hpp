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

      .execute = [](const Instruction& inst, ProcessData& proc) -> Uint {
        auto& prog = proc.get_program();
        auto& context  = prog.get_context();

        // Check for matching FOR context
        if (context.empty())
          throw runtime_error("[ENDFOR] No matching FOR context.");

        auto& frame = context.top();

        // First time hitting ENDFOR, set jump address
        if (frame.jump_addr == 0)
          context.set_jump(prog.next_ip());
        
         // Decrement count and check if we're done
        if (--frame.count > 0)
          return frame.line_addr;

        // Finished with this loop
        context.pop(); 
        return nullopt;
      },

      .example = [](ProcessData&) -> Instruction {
        return { "ENDFOR" };
      },
    };
  }
}
