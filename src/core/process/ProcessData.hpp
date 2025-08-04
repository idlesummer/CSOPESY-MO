#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/memory/MemoryView.hpp"
#include "ProcessMemory2.hpp"
#include "ProcessProgram.hpp"
#include "ProcessControl.hpp"
#include "types.hpp"


/**
 * @brief Pure data container for process state.
 * 
 * This class holds all memory, program, state, and metadata
 * related to a single process. It is only accessible by the
 * Process class, which owns and manipulates this data.
 */
class ProcessData {
  public:

  /** @brief Constructs a process given name and process ID. */
  ProcessData(uint id, str name, MemoryView view, vec<Instruction> script): 
    id      (id),                         // Unique process ID
    name    (move(name)),                 // Human-readable process name (e.g. p01, p02)
    logs    (vec<str>()),                 // Output logs collected from PRINT instructions
    core_id (0u),                         // ID of the core this process is assigned to (0 if unassigned)
    stime   (Clock::now()),               // Timestamp of when the process was created
    memory  (ProcessMemory(move(view))),  // TODO: docs
    program (ProcessProgram(script)),     // List of instructions and execution context
    control (ProcessControl()) {}         // Sleep controller of the process

  /** @brief Append a log message (used for PRINT instructions) */
  void log(str line) { logs.push_back(move(line)); }

  // ------ Instance variables ------
  uint id;            
  str name;          
  vec<str> logs;         
  uint core_id;       
  Time stime;        
  ProcessMemory memory;   
  ProcessProgram program; 
  ProcessControl control; 
};
