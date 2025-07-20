#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"


inline InstructionHandler make_declare() {
  using Param = InstructionParam;
  using list = vector<str>;
  return {
    .opcode = "DECLARE",
    .signatures = {{ Param::Var(), Param::UInt16() }},
    .execute = [](Instruction& inst, ProcessData& proc) {
      uint value = stoul(inst.args[1]);
      proc.get_memory().set(inst.args[0], value);
    },
  };
}

