#pragma once
#include "core/common/imports/_all.hpp"
#include "ProcessState.hpp"
#include "ProcessMemory.hpp"
#include "ProcessProgram.hpp"
#include "ProcessData.hpp"
#include "ProcessExecutor.hpp"

namespace csopesy {

  class Process {
    using list = ProcessData::list;
    ProcessData data;

    public:
    Process(str name, uint pid) : data(move(name), pid) {}

    // === ProcessExecutor interface ===
    bool step() { return ProcessExecutor::step(data); }

    // === ProcessData interface ===

    // Mutators for ProcessData components
    void log(str line) { data.log(move(line)); }
    void set_core(int id) { data.set_core(id); }
    void reset_core() { data.reset_core(); }

    // Accessors for ProcessData components
    const str& get_name() const { return data.get_name(); }
    const uint get_id() const { return data.get_id(); }
    const int  get_core() const { return data.get_core(); }
    const Time& get_stime() const { return data.get_stime(); }
    const list& get_logs() const { return data.get_logs(); }

    // Accessors for ProcessData sub-components
    ProcessData& get_data() { return data; }
    ProcessState& get_state() { return data.get_state(); }
    ProcessMemory& get_memory() { return data.get_memory(); }
    ProcessProgram& get_program() { return data.get_program(); }
    const ProcessData& get_data() const { return data; }
    const ProcessState& get_state() const { return data.get_state(); }
    const ProcessMemory& get_memory() const { return data.get_memory(); }
    const ProcessProgram& get_program() const { return data.get_program(); }

    /** Creates a random process with the given name. */
    static Process create(str name, uint pid, Instruction::Script script) {
      Process proc(std::move(name), pid);
      proc.get_program().load_script(move(script));
      return proc;
    }
  };
}
