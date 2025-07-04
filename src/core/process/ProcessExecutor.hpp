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
      
      interpreter.execute(inst, proc);
      
      // This line checks if IP was changed. If not, advance it.
      if (ip == prog.get_ip())
        prog.set_ip(ip + 1);

      return false;
    }
  };
}
