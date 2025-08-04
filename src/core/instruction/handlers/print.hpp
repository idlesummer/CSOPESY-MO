#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/instruction/types.hpp"
#include "core/process/ProcessData.hpp"


auto make_print() -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("PRINT")
    // .add_signature(Signature().Str())
    // .add_signature(Signature().Str().Var())

    .set_execute([](Instruction& inst, ProcessData& process) {
      process.log(format("Hello world from {}!", process.name));
    });
}
