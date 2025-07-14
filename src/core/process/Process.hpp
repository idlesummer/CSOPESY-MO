#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionInterpreter.hpp"
#include "ProcessData.hpp"

namespace csopesy {

  class Process {
    using Interpreter = InstructionInterpreter;
    using Script = Instruction::Script;
    inline static Interpreter& interpreter = Interpreter::instance();
    
  public:
    // === Components ===
    ProcessData data;         // Data container of process

    // === Methods ===
    Process(uint pid, str name, Script script): 
      data(pid, move(name), move(script)) {}
    
    /** @brief Appends a log message (e.g. from PRINT instruction). */
    void log(str line) { data.log(move(line)); }

    /** @brief Executes a single instruction step for the given process. */
    bool step() {
      if (data.state.finished() || data.program.finished()) {
        data.state.finish();  // Mark as finished
        return true;          // Process is done
      }

      auto ip = data.program.ip;
      auto& inst = data.program.script.at(ip);
      interpreter.execute(inst, data);

      // Advance to next instruction if IP unchanged
      data.program.ip += (ip == data.program.ip);

      return false;
    }

    /** @brief Creates a process with a random instruction script (to be implemented). */
    static Process create(uint pid, str name, uint size) {
      auto script = interpreter.generate_script(size);
      return Process(pid, move(name), move(script));
    }
  };
}
