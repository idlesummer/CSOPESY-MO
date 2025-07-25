#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionParam.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"

namespace csopesy::instruction {
  inline InstructionHandler make_for() {
    using Param = InstructionParam;
    
    // === Skips ahead to the matching ENDFOR. ===
    auto skip_block = [](auto& program, const Instruction& inst) {
      
      // Use cached exit if available
      if (inst.exit != 0)
        return program.set_ip(inst.exit);
      
      auto start = program.get_ip();
      auto& script = program.get_script();
      uint depth = 1;

      // Otherwise, scan ahead to find the matching ENDFOR
      for (uint i = start+1; i < script.size(); ++i) {
        const auto& opcode = script[i].opcode;
        const int delta = (opcode == "FOR") - (opcode == "ENDFOR");
        depth += delta;
        
        // If matching ENDFOR is found, cache exit address and exit loop
        if (depth == 0) {
          inst.exit = i+1;  
          program.set_ip(inst.exit);
          return;
        }
      }
      
      throw runtime_error("[FOR] Mismatched FOR/ENDFOR blocks.");
    };

    return {
      .opcode = "FOR",
      .exit_opcode = "ENDFOR",
      .signatures = {{ Param::UInt(1, 5) }},
      .execute = [&](const Instruction& inst, ProcessData& process) {
        auto& program = process.get_program();
        auto count = stoul(inst.args[0]);
        
        // Skip loop entirely if loop count is zero
        if (count == 0)
          return skip_block(program, inst);
        
        auto& context = program.get_context();
        auto ip = program.get_ip();
        
        // Push context if this FOR hasn't been visited yet
        if (!context.starts_at(ip))
          context.push("FOR", ip, count);
      },
    };
  }
}
