#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionInterpreter.hpp"
#include "ProcessData.hpp"


class Process {
  using Interpreter = InstructionInterpreter;
  using Script = Instruction::Script;
  inline static Interpreter& interpreter = Interpreter::instance();
  
  public:
  // === Components ===
  ProcessData data;         // Data container of process

  // === Methods ===

  /** @brief Creates a process with a random instruction script. */
  Process(uint pid, str name, uint size): 
    data(pid, move(name), move(interpreter.generate_script(size))) {}
  
  /** @brief Appends a log message (e.g. from PRINT instruction). */
  void log(str line) { data.log(move(line)); }

  /** @brief Executes a single instruction step for the given process. */
  bool step() {
    if (data.program.finished())
      return true;          // Process is done

    auto ip = data.program.ip;
    auto& inst = data.program.script.at(ip);
    interpreter.execute(inst, data);

    // Advance to next instruction if IP unchanged
    if (ip == data.program.ip)
      data.program.ip++ ;

    return false;
  }
};
