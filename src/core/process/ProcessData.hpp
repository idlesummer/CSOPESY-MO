#pragma once
#include "core/common/imports/_all.hpp"
#include "ProcessMemory.hpp"
#include "ProcessProgram.hpp"
#include "ProcessState.hpp"
#include "types.hpp"

namespace csopesy {

  /**
   * @brief Pure data container for process state.
   * 
   * This class holds all memory, program, state, and metadata
   * related to a single process. It is only accessible by the
   * Process class, which owns and manipulates this data.
   */
  class ProcessData {
    public:
    using list = vector<str>;

    private:
    str name;               // Human-readable process name (e.g. p01, p02)
    uint id;                // Unique process ID
    int core = -1;          // ID of the core this process is assigned to (-1 if unassigned)
    list logs;              // Output logs collected from PRINT instructions
    Time stime;             // Timestamp of when the process was created
    ProcessState state;     // Current state of the process (Ready, Running, Sleeping, Finished)
    ProcessMemory memory;   // Key-value variable store (e.g. for DECLARE, ADD, etc.)
    ProcessProgram program; // List of instructions and execution context

    public:

    /** Constructs a process with a given name and process ID. */
    ProcessData(str name, uint id): 
      name(move(name)), id(id), stime(Clock::now()) {}

    /** Append a log message (used for PRINT instructions) */
    void log(str line) { logs.push_back(move(line)); }

    // === Core mutators ===
    void set_core(int id) { core = id; }
    void reset_core() { core = -1; }

    // === Getters ===
    const str& get_name() const { return name; }
    const uint get_id() const { return id; }
    const int get_core() const { return core; }
    const Time& get_stime() const { return stime; }
    const list& get_logs() const { return logs; }

    // === Accessors for sub-components ===
    ProcessState& get_state() { return state; }
    ProcessMemory& get_memory() { return memory; }
    ProcessProgram& get_program() { return program; }
    const ProcessState& get_state() const { return state; }
    const ProcessMemory& get_memory() const { return memory; }
    const ProcessProgram& get_program() const { return program; }
  };
}
