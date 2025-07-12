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
      .open_opcode = "FOR",
      .execute = [](const Instruction& inst, ProcessData& process) {
        auto& program = process.program;
        auto& context = program.context;

        // Check if block is inside a FOR loop
        if (!context.matches("FOR"))
          throw runtime_error("[ENDFOR] No matching FOR block on stack.");
        
        auto& frame = context.top();

        // Cache exit address if it's not set
        auto& for_inst = program.script.at(frame.start);
        if (for_inst.exit == 0)
          for_inst.exit = program.ip + 1;

        // Decrement loop count; jump back if more iterations remain
        if (--frame.count > 0)
          return void(program.ip = frame.start);

        // Loop finished - pop the context frame
        context.pop();
      },
    };
  }
}
