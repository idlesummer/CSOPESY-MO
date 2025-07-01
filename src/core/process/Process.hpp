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

    str name;
    uint pid;
    int core = -1;
    Time start_time;
    ProcessState state;
    ProcessMemory memory;
    ProcessProgram program;
    list logs;
    
    public:
    Process(str name, uint pid): name(move(name)), pid(pid), start_time(Clock::now()) {}
    
    /** Helpers */
    void log(str line) { logs.push_back(move(line)); }

    /** Getters only (no more logic here) */
    const str& get_name() const { return name; }
    uint get_pid() const { return pid; }
    int get_core() const { return core; }
    const Time& get_start_time() const { return start_time; }
    const list& get_logs() const { return logs; }

    ProcessState& get_state() { return state; }
    const ProcessState& get_state() const { return state; }
    
    ProcessMemory& get_memory() { return memory; }
    const ProcessMemory& get_memory() const { return memory; }
    
    ProcessProgram& get_program() { return program; }
    const ProcessProgram& get_program() const { return program; }

    void set_core(int id) {
      core = id;
    }

    void clear_core() {
      core = -1;
    }
  };
}
