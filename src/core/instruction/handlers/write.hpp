#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"

auto make_write () -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("WRITE")
    .add_signature(Signature().Uint16().Uint16())
    
    .set_execute([](Instruction& inst, ProcessData& process) {
      auto& program = process.program;
      auto& memory = process.memory;

      auto vaddr = stoul(inst.args[0], nullptr, 0); 
      auto value = stoul(inst.args[1], nullptr, 0);

      auto [is_violation, is_page_fault] = memory.virtual_memory.write(vaddr, value);

      if (is_violation) {
        process.log(format("[WRITE] access violation at address 0x{:X}", vaddr));
        program.terminate();
        return;
      }

      if (is_page_fault) {
        process.log(format("[WRITE] Unable to resolve page fault on first try for '{}'", value));
        program.set_ip(program.ip);
        return;
      }
    });
}