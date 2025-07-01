#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/types.hpp"
#include "ProcessMemory.hpp"
#include "ProcessProgram.hpp"
#include "ProcessState.hpp"
#include "types.hpp"

namespace csopesy {
  class Process {
    using list = vector<str>;

    str name;               // Human-readable process name (e.g. p01, p02)
    uint pid;               // Unique process ID
    int core = -1;          // ID of the core this process is assigned to (-1 if unassigned)
    Time start_time;        // Timestamp of when the process was created
    ProcessState state;     // Current state of the process (Ready, Running, Sleeping, Finished)
    ProcessMemory memory;   // Key-value variable store (e.g. for DECLARE, ADD, etc.)
    ProcessProgram program; // List of instructions and execution context
    list logs;              // Output logs collected from PRINT instructions
    
    public:

    /** Constructs a process with a given name and process ID. */
    Process(str name, uint pid): name(move(name)), pid(pid), start_time(Clock::now()) {}
    
    /** Append a log message (used for PRINT instructions) */
    void log(str line) { logs.push_back(move(line)); }
    
    // === Core binding helpers ===
    void set_core(int id) { core = id; }
    void reset_core() { core = -1; }

    // === Getters ===
    const str& get_name() const { return name; }
    const uint get_pid() const { return pid; }
    const int  get_core() const { return core; }
    const Time& get_start_time() const { return start_time; }
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
