#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"
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
    using Cores = CoreManager;
    using ProcessPtr = unique_ptr<Process>;
    using queue = queue<uint>;
    using map = unordered_map<uint, ProcessPtr>;
    using list = vector<uint>;

    auint next_pid;         ///< Monotonic PID counter for generating unique process IDs
    
    public:
    SchedulerConfig config; ///< Runtime configuration settings
    Cores cores;            ///< Reference to the list of scheduler cores
    map processes;          ///< Container for all processes
    queue rqueue;           ///< Ready queue of processes waiting to be scheduled
    list finished_pids;     ///< PIDs of finished processes 

    // === Methods ===

    /** @brief Constructs a SchedulerData context with the given number of cores. */
    SchedulerData(uint size=1): next_pid(1) {
      cout << "[SchedulerData::constructor()] Called.";
    }

    /** @brief Sets configuration and resizes cores. */
    void set_config(SchedulerConfig new_config) {
      config = move(new_config);
      // cores.resize(config.num_cpu);
    }

    /** @brief Returns a unique, incrementing process ID. */
    uint new_pid() { return next_pid++; }

    /** @brief Adds a process to the process table. */
    void add_process(Process proc) {
      processes.emplace(proc.data.id, make_unique<Process>(move(proc)));
    }

    /** @brief Check if a process with the given ID exists in the process table. */
    bool has_process(uint id) const { return processes.contains(id); }
      
    /** @brief Check if a process with the given name exists in the process table. */
    bool has_process(const str& name) const { return !!find_process_by_name(name, false); }

    // === Accessors ===
    
    /** @brief Returns a reference wrapper to the process with the given PID. */
    Process& get_process(uint id) { return *processes.at(id); }

    /** @brief Returns a reference wrapper to the process with the given name. */
    Process& get_process(const str& name) { return *find_process_by_name(name); }
    
    /** @brief Returns the list of running process IDs. */
    list get_running_pids() { return cores.get_running_pids(); }

    private:

    // === Helpers ===
  
    Process* find_process_by_name(const str& name, bool throw_if_missing=true) const {
      for (const auto& [_, proc] : processes)
        if (proc->data.name == name)
          return proc.get();
    
      if (throw_if_missing)
        throw runtime_error(format("Process with name '{}' not found.", name));
      return nullptr;
    }
  };   
}
