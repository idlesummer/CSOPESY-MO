#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionInterpreter.hpp"
#include "ProcessData.hpp"


class Process {
  public:  

  /** @brief Creates a process with a random instruction script. */
  Process(uint pid, str name, uint size): 
    data(           // Data container of process
      ProcessData(
        pid,
        move(name),
        move(interpreter.generate_script(size))
      )
    ) {}
  
  /** @brief Appends a log message (e.g. from PRINT instruction). */
  void log(str line) { data.log(move(line)); }

  /** @brief Executes a single instruction step for the given process. */
  auto step() -> bool {
    auto& program = data.program;
    program.block = false;        // Reset blocking instruction

    if (program.finished())
      return true;          
      
    auto ip = program.ip; 
    auto& inst = data.program.script.at(ip);
    interpreter.execute(inst, data);

    // Only auto-advance if not blocked and IP unchanged
    if (!program.block && ip == data.program.ip)
      data.program.ip++;
      
    return false;
  }

  // ------ Member variables ------
  inline static auto& interpreter = InstructionInterpreter::get();
  ProcessData data;
};
