#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/instruction/types.hpp"
#include "core/process/ProcessData.hpp"


auto make_print() -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("PRINT")
    .add_signature(Signature().strT())
    .add_signature(Signature().strT().varT())
    .set_execute([](Instruction& inst, ProcessData& proc) {
      proc.log(inst.args[0]);
    });
}
