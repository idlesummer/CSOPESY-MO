#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionParam.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"

namespace csopesy::instruction {
  inline InstructionHandler make_sleep() {
    using Param = InstructionParam;
    return {
      .opcode = "SLEEP",
      .signatures = {{ Param::UInt8() }},
      .execute = [](const Instruction& inst, ProcessData& proc) {
        uint duration = stoul(inst.args[0]);
        proc.get_state().sleep_for(duration);
      },
    };
  }
}
