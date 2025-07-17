#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "ProcessMemory.hpp"
#include "ProcessProgram.hpp"
#include "ProcessState.hpp"
#include "types.hpp"


/**
 * @brief Pure data container for process state.
 * 
 * This class holds all memory, program, state, and metadata
 * related to a single process. It is only accessible by the
 * Process class, which owns and manipulates this data.
 */
class ProcessData {
  using Script = Instruction::Script;
  using list = vector<str>;

public:
  // === Identity ===
  uint id = 0;            // Unique process ID
  str name = "";          // Human-readable process name (e.g. p01, p02)
  
  // === Properties ===
  list logs = {};         // Output logs collected from PRINT instructions
  
  // === Scheduling/Execution Metadata ===
  int core_id = -1;       // ID of the core this process is assigned to (-1 if unassigned)
  Time stime = {};        // Timestamp of when the process was created
  
  // === Components ===
  ProcessState state;     // Current state of the process (Ready, Running, Sleeping, Finished)
  ProcessMemory memory;   // Key-value variable store (e.g. for DECLARE, ADD, etc.)
  ProcessProgram program; // List of instructions and execution context

  // === Methods ===

  /** @brief Constructs a process given name and process ID. */
  ProcessData(uint id, str name, Script script): 
    id(id), name(move(name)), program(script), stime(Clock::now()) {}

  /** @brief Append a log message (used for PRINT instructions) */
  void log(str line) { logs.push_back(move(line)); }
};
