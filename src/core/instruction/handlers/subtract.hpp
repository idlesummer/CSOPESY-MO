#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"


auto make_subtract() -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("SUBTRACT")
    .add_signature(Signature().Var().Var().Var())
    .add_signature(Signature().Var().Var().Uint16())
    .add_signature(Signature().Var().Uint16().Uint16())

    .set_execute([](Instruction& inst, ProcessData& process) {
      auto& memory = process.memory;
      auto lhs = memory.get(inst.args[1]);
      auto rhs = memory.resolve(inst.args[2]);
      memory.set(inst.args[0], lhs > rhs ? lhs - rhs : 0);
    });
}
