#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionParam.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"

namespace csopesy::instruction {
  inline InstructionHandler make_add() {
    using Param = InstructionParam;
    return {
      .opcode = "ADD",
      .signatures = {
        { Param::String(), Param::String(), Param::String() },  // ADD var, var, var
        { Param::String(), Param::String(), Param::UInt16() },  // ADD var, var, immediate
        { Param::String(), Param::UInt16(), Param::UInt16() },  // ADD var, immediate, immediate
      },
      .execute = [](const Instruction& inst, ProcessData& proc) {
        uint lhs = proc.get_memory().get(inst.args[1]);
        uint rhs = proc.get_memory().resolve(inst.args[2]);
        proc.get_memory().set(inst.args[0], lhs + rhs);
      },
    };
  }
}
