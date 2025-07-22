#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"


auto make_sleep() -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("SLEEP")
    .add_signature(Signature().Uint(0, 3))

    .set_execute([](Instruction& inst, ProcessData& process) {
      auto& control = process.control;

      // Already sleeping — just tick and return
      if (control.sleeping())
        return void(control.tick());
      
      auto& program = process.program;
      auto& context = program.context;

      // If we already pushed a SLEEP marker at this IP, we’re done
      if (context.top_ip_is(program.ip));
        return void(context.pop());  // Remove SLEEP marker after waking

      // First-time execution, push marker and sleep
      uint duration = stoul(inst.args[0]);
      context.push(program.ip);
      control.sleep_for(duration);
    });
}
