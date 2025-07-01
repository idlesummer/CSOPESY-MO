#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"
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

    static bool step(Process& data) {
      auto& state = data.get_state();
      auto& prog  = data.get_program();
      auto& ctx   = prog.get_context();
      auto& insts = prog.get_instructions();

      if (state.state == State::Finished)
        return true;

      // Handle sleeping
      if (state.is_sleeping()) {
        if (--state.sleep_ticks == 0)
          state.set_ready();
        return false;
      }

      // Top-level instruction stream
      if (ctx.empty()) {
        if (prog.get_ip() >= insts.size()) {
          state.set_finished();
          return true;
        }

        const auto& inst = insts[prog.next_ip()];
        if (inst.opcode == "FOR")
          ctx.emplace_back(inst);
        else
          Interpreter::instance().execute(inst, data);

        return prog.is_finished();
      }

      // Inside FOR block
      auto& frame = ctx.back();

      if (frame.end_of_block()) {
        frame.next_iteration();
        if (frame.should_exit())
          ctx.pop_back();
        return false;
      }

      const auto& sub_inst = frame.block.get()[frame.ip++];
      if (sub_inst.opcode == "FOR")
        ctx.emplace_back(sub_inst);
      else
        Interpreter::instance().execute(sub_inst, data);

      if (prog.is_finished()) {
        state.set_finished();
        return true;
      }

      return false;
    }
  };
}
