#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionParam.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"


inline InstructionHandler make_subtract() {
  using Param = InstructionParam;
  return {
    .opcode = "SUBTRACT",
    .signatures = {
      { Param::Var(), Param::Var(),    Param::Var() },    // ADD var, var, var
      { Param::Var(), Param::Var(),    Param::UInt16() }, // ADD var, var, val
      { Param::Var(), Param::UInt16(), Param::UInt16() }, // ADD var, val, val
    },
    .execute = [](Instruction& inst, ProcessData& proc) {
      uint lhs = proc.get_memory().get(inst.args[1]);
      uint rhs = proc.get_memory().resolve(inst.args[2]);
      proc.get_memory().set(inst.args[0], (lhs > rhs) ? (lhs-rhs) : 0);
    },
  };
}
