#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/InstructionInterpreter.hpp"
#include "ProcessData.hpp"

namespace csopesy {

  class Process {
    using Interpreter = InstructionInterpreter;
    inline static Interpreter& interpreter = Interpreter::instance();
    
    public:
    ProcessData data; ///< Data container of process

    // === Methods ===
    Process(uint pid, str name): data(move(name), pid) {}
    
    /** @brief Appends a log message (e.g. from PRINT instruction). */
    void log(str line) { data.log(move(line)); }

    /** @brief Executes a single instruction step for the given process. */
    bool step() {
      if (data.state.finished() || data.program.finished())
        return data.state.finish(), true;

      auto ip = data.program.ip;
      auto& inst = data.program.script.at(ip);

      interpreter.execute(inst, data);

      if (ip == data.program.ip)
        data.program.ip = ip + 1;

      return false;
    }

    /** @brief Creates a process with a random instruction script (to be implemented). */
    static Process create(uint pid, str name, uint size) {
      auto process = Process(pid, move(name));
      auto script = interpreter.generate_script(size);
      process.data.program.load_script(script);
      return move(process);
    }
  };
}
