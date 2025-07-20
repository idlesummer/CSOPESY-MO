#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionParam.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"


auto make_for() -> InstructionHandler {
  // === Skips ahead to the matching ENDFOR. ===
  auto skip_block = [](auto& program, auto& inst) {
    
    // Use cached exit if available
    if (inst.exit != 0)
      return void(program.ip = inst.exit);
    
    auto start = program.ip;
    auto& script = program.script;
    uint depth = 1;

    // Otherwise, scan ahead to find the matching ENDFOR
    for (uint i = start+1; i < script.size(); ++i) {
      auto& opcode = script[i].opcode;
      int delta = (opcode == "FOR") - (opcode == "ENDFOR");
      depth += delta;
      
      // If matching ENDFOR is found, cache exit address and exit loop
      if (depth == 0) {
        program.ip = inst.exit = i + 1;
        return;
      }
    }
    
    throw runtime_error("[FOR] Mismatched FOR/ENDFOR blocks.");
  };

  return InstructionHandler()
    .set_opcode("FOR")
    .set_exit_opcode("ENDFOR")
    .add_signature({ InstructionParam::UInt(1, 5) })
    .set_execute([&](Instruction& inst, ProcessData& proc) {
      auto& program = proc.program;
      auto count = stoul(inst.args[0]);
      if (count == 0)
        return skip_block(program, inst);  // Skip if count is 0

      auto& context = program.context;
      if (!context.starts_at(program.ip))
        context.push("FOR", program.ip, count);
    });
}
