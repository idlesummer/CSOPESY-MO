#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"


auto make_endfor() -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("ENDFOR")
    .set_open("FOR")

    .set_execute([](Instruction& inst, ProcessData& process) {
      auto& program = process.program;
      auto& context = program.context;
      auto& frame = context.top();
      auto& frame_inst = program.script.at(frame.ip);

      // Check if block is inside a FOR loop
      if (frame_inst.opcode != "FOR")
        throw runtime_error("[ENDFOR] No matching FOR block on stack.");

      // Cache exit address if it's not set
      if (inst.exit == 0)
        inst.exit = program.ip + 1;

      // Decrement loop count; jump back if more iterations remain
      if (--frame.ctr > 0)
        return void(program.set_ip(frame.ip));

      context.pop();
    });
}
