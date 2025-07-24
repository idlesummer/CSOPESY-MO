#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/instruction/types.hpp"
#include "core/process/ProcessData.hpp"


auto make_for() -> InstructionHandler {
  
  // === Skips ahead to the matching ENDFOR. ===
  auto skip_block = [](auto& program, auto& inst) { 

    cout << "Before: inst.exit = " << inst.exit << "\n";
    
    // Use cached exit if available
    if (inst.exit != 0)
      return void(program.set_ip(inst.exit));
      
    auto& script = program.script;
    uint start = program.ip;
    uint depth = 1;


    // Otherwise, scan ahead to find the matching ENDFOR
    for (uint i = start+1; i < script.size(); ++i) {
      auto& opcode = script[i].opcode;
      int delta = (opcode == "FOR") - (opcode == "ENDFOR");
      depth += delta;
      
      // If matching ENDFOR is found, cache exit address and exit loop
      if (depth == 0) {
        inst.exit = i + 1;
        program.set_ip(inst.exit);
        cout << "After: inst.exit = " << inst.exit << "\n";
        return;
      }
    }
    
    throw runtime_error("[FOR] Mismatched FOR/ENDFOR blocks.");
  };

  return InstructionHandler()
    .set_opcode("FOR")
    .set_exit("ENDFOR")
    .add_signature(Signature().Uint(1, 5))

    .set_execute([&](Instruction& inst, ProcessData& process) {
      auto& program = process.program;
      auto count = stoul(inst.args[0]);

      // Skip if counter is 0
      if (count == 0)
        return skip_block(program, inst);

      auto& context = program.context;
      if (!context.top_ip_is(program.ip))
        context.push(program.ip, count);
    });
}
