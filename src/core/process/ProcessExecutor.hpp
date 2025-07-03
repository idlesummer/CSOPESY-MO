#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/ProcessData.hpp"
#include "core/instruction/InstructionInterpreter.hpp"

namespace csopesy {
  /**
   * @brief Executes one step of a process.
   * 
   * ProcessExecutor is a stateless utility that performs a single execution step
   * for a given ProcessData object. It handles sleeping, instruction execution,
   * and FOR loop semantics.
   */
  class ProcessExecutor {
    using Interpreter = InstructionInterpreter;
    inline static Interpreter interpreter = Interpreter::instance();

    public:  
    static bool step(ProcessData& proc) {
      auto& prog  = proc.get_program();
      auto& state = proc.get_state();

      // Exit if program already finished
      if (state.is_finished() || prog.is_finished())
        return state.set_finished(), true;

      // Fetch and execute instruction
      const auto ip = prog.get_ip();
      const auto& inst = prog.get_instructions()[ip];
      const auto next = interpreter.execute(inst, proc);
      
      const auto line = next.value_or(prog.next_ip());
      prog.set_ip(line);

      return false;
    }
  };
}
