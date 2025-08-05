#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"

auto make_read () -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("READ")
    .add_signature(Signature().Var().Uint16())

    .set_execute([](Instruction& inst, ProcessData& process) {
      auto& program = process.program;
      auto& memory = process.memory;

      auto var   = inst.args[0];
      auto vaddr = stoul(inst.args[1], nullptr, 0); // parses hex or dec

      auto [value, is_violation, is_page_fault] = memory.virtual_memory.read(vaddr);

      if (is_violation) {
        process.log(format("[READ] access violation at address 0x{:X}", vaddr));
        program.terminate();
        return;
      }

      if (is_page_fault) {
        process.log(format("[READ] Unable to resolve page fault on first try for '{}'", var));
        program.set_ip(program.ip);
        return;
      }

      memory.set(var, value);
    });
}