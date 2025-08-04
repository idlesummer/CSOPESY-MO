#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"


auto make_sleep() -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("SLEEP")
    .add_signature(Signature().Uint(0,3))

    .set_execute([](Instruction& inst, ProcessData& process) {
      auto ticks = stoul(inst.args[0]);

      // Case 0: Skip sleep if duration is zero
      if (ticks == 0)
        return;

      auto& program = process.program;
      auto& control = process.control;

      // Case 1: Begin or continue sleeping
      if (!control.sleeping())
        control.sleep_for(ticks);
      else 
        control.tick();

      // Case 2: Still sleeping
      if (control.sleeping())
        program.set_ip(program.ip);  // Block auto-increment
    });
}
