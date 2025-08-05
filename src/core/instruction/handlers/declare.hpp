#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"


auto make_declare() -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("DECLARE")
    .add_signature(Signature().Var().Uint16())

    .set_execute([](Instruction& inst, ProcessData& process) {
      auto& program = process.program;
      auto var   = inst.args[0];
      auto value = stoul(inst.args[1], nullptr, 0);
      auto [is_violation, is_page_fault, is_full] = process.memory.set(var, value);

      if (is_violation) {
        process.log(format("[DECLARE] write violation for '{}'", var));
        // program.terminate();
        // return;
      }
      else if (is_page_fault) {
        process.log(format("[DECLARE] Unable to resolve page fault on first try for '{}'", var));
        program.set_ip(program.ip);
        return;
      }
      else if (is_full)
        process.log(format("[DECLARE] failed: symbol table full → '{}'", var));
    });
}
