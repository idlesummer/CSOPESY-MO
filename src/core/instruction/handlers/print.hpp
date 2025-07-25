#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionParam.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"

namespace csopesy::instruction {
  inline InstructionHandler make_print() {
    using Param = InstructionParam;
    return {
      .opcode = "PRINT",
      .signatures = {
        { Param::Str() },
        { Param::Str(), Param::Var()},
      },
      .execute = [](const Instruction& inst, ProcessData& proc) {
        proc.log(inst.args[0]);
      },
    };
  }
}
