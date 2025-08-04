#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionInterpreter.hpp"
#include "core/memory/MemoryView.hpp"
#include "ProcessData.hpp"


class Process {
  public:  
  // ------ Class variables ------
  inline static auto& interpreter = InstructionInterpreter::get();

  /** @brief Creates a process with a random instruction script. */
  Process(uint pid, str name, uint size, MemoryView& view): 
    data(           // Data container of process
      ProcessData(
        pid,
        move(name),
        move(interpreter.generate_script(size)),
        view
      )
    ) {}

  /** @brief Executes a single instruction step for the given process. */
  auto step() -> bool {
    auto& program = data.program;
    if (program.finished())
      return true;          
      
    auto ip = program.ip; 
    auto& inst = data.program.script.at(ip);
    interpreter.execute(inst, data);

    // Don't auto-advance ip if it was not manually set
    if (!program.ip_was_set)
      program.ip++;
    
    program.ip_was_set = false;
    return false;
  }

  // ------ Instance variables ------
  ProcessData data;
};
