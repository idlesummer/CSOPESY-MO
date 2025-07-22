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
    if (data.program.finished())
      return true;          
      
    auto ip = data.program.ip;
    auto& inst = data.program.script.at(ip);
    interpreter.execute(inst, data);

    // Advance to next instruction if IP unchanged
    if (ip == data.program.ip)
      data.program.ip++;

    return false;
  }

  // ------ Member variables ------
  inline static auto& interpreter = InstructionInterpreter::get();
  ProcessData data;
};
