#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"


auto make_endfor() -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("ENDFOR")
    .set_open_opcode("FOR")
    .set_execute([](Instruction& inst, ProcessData& process) {
      auto& program = process.program;
      auto& context = program.context;

      // Check if block is inside a FOR loop
      if (!context.matches("FOR"))
        throw runtime_error("[ENDFOR] No matching FOR block on stack.");

      auto& frame = context.top();
      auto& for_inst = program.script.at(frame.start);

      // Cache exit address if it's not set
      if (for_inst.exit == 0)
        for_inst.exit = program.ip + 1;

      // Decrement loop count; jump back if more iterations remain
      if (--frame.count > 0)
        return void(program.ip = frame.start);

      context.pop();
    });
}
