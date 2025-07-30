#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"


auto make_declare() -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("DECLARE")
    .add_signature(Signature().Var().Uint16())
    .set_execute([](Instruction& inst, ProcessData& process) {
      auto var = inst.args[0];
      auto value = stoul(inst.args[1]);
      process.memory.set(var, value);
    });
}
