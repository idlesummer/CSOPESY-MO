#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"


auto make_sleep() -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("SLEEP")
    .add_signature(Signature().Uint(0, 3))

    .set_execute([](Instruction& inst, ProcessData& process) {
      auto& program = process.program;
      auto& control = process.control;
      auto& context = program.context;

      // Phase 1: First-time setup
      if (!context.top_ip_is(program.ip)) {
        context.push(program.ip);
        control.sleep_for(stoul(inst.args[0]));
      }

      // Phase 2: Sleep body
      if (!control.sleeping())  // If not sleeping anymore, advance
        context.pop();        

      else {                    // If still sleeping, block ip advancing
        control.tick();
        program.move_ip(0);
      }
    });
}
