#pragma once
#include "core/common/imports/_all.hpp"
#include "core/common/utility/EventEmitter.hpp"
#include "core/process/Process.hpp"
#include "core/execution/Core.hpp"
#include "core/execution/CoreManager.hpp"
#include "types.hpp"

namespace csopesy {

  /**
   * @brief Encapsulates scheduling context passed to strategy functions.
   * 
   * Provides access to the current core list, ready queue, and tick counter.
   * Also handles state reset and tick progression during simulation.
   */
  class SchedulerData {
    using ProcessCref = Core::ProcessCref;
    using ProcessCrefs = vector<ProcessCref>;
    using ProcessRef = Core::ProcessRef;
    using ProcessPtr = unique_ptr<Process>;
    using Cores = CoreManager;

    using queue = queue<uint>;
    using map = unordered_map<uint, ProcessPtr>;
    using set = unordered_set<uint>;
    using list = vector<uint>;

    SchedulerConfig config; ///< Runtime configuration settings
    map processes;          ///< Container for all processes
    queue rqueue;           ///< Ready queue of processes waiting to be scheduled
    Cores cores;            ///< Reference to the list of scheduler cores
    set running;            ///< PIDs of currently running processes
    list finished;          ///< PIDs of finished processes 
    auint next_pid;         ///< Monotonic PID counter for generating unique process IDs

    public:

    /** Constructs a SchedulerData context with the given number of cores. */
    SchedulerData(uint size=1): cores(size) {}

    /** Sets configuration and resizes cores. */
    void set_config(SchedulerConfig new_config) {
      config = move(new_config);
      cores.resize(config.num_cpu);
    }

    /** Returns a unique, incrementing process ID. */
    uint generate_pid() { return next_pid++; }

    /** Adds a process to the process table. */
    void add_process(Process proc) {
      processes.emplace(proc.get_id(), make_unique<Process>(move(proc)));
    }

    /** Check if a process with the given ID exists in the process table. */
    bool has_process(uint id) const { return processes.contains(id); }
      
    /** Check if a process with the given name exists in the process table. */
    bool has_process(const str& name) const {
      return any_of(processes, [&](const auto& pair) {
        return pair.second->get_name() == name;
      });
    }

    // === Component Accessors ===
    
    /** Returns a reference wrapper to the process with the given PID. */
    ProcessRef get_process(uint id) { return ref(*processes.at(id)); }
    ProcessCref get_process(uint id) const { return cref(*processes.at(id)); }

    /** Returns a reference wrapper to the process with the given name. */
    ProcessRef get_process(const str& name) { return ref(find_process_by_name(name)); }
    ProcessCref get_process(const str& name) const { return cref(find_process_by_name(name)); }
    
    /** Returns the scheduler configuration. */
    SchedulerConfig& get_config() { return config; }
    const SchedulerConfig& get_config() const { return config; }

    /** Returns the number of processes in the global table. */
    uint get_process_count() const { return processes.size(); }

    // === Core Accessors ===

    /** Core Access */ 
    uint get_core_count() const { return cores.size(); }
    Cores& get_cores() { return cores; }
    const Cores& get_cores() const { return cores; }

    /** Queue Access */ 
    queue& get_rqueue() { return rqueue; }
    const queue& get_rqueue() const { return rqueue; }

    /** Process Table Access */
    map& get_processes() { return processes; }
    const map& get_processes() const { return processes; }

    /** Returns the set of running process IDs. */
    set& get_running() { return running; }
    const set& get_running() const { return running; }
    
    /** Returns the list of finished process IDs. */
    list& get_finished() { return finished; }
    const list& get_finished() const { return finished; }
    
    private:

    // === Helpers ===
  
    Process& find_process_by_name(const str& name) {
      for (auto& [_, proc]: processes)
        if (proc->get_name() == name) return *proc;
      throw runtime_error(format("Process with name '{}' not found.", name));
    }

    const Process& find_process_by_name(const str& name) const {
      for (const auto& [_, proc]: processes)
        if (proc->get_name() == name) return *proc;
      throw runtime_error(format("Process with name '{}' not found.", name));
    }
  };   
}
