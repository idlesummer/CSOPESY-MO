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

    auint next_pid = 1;       ///< Monotonic PID counter for generating unique process IDs
    
    public:
    list finished_pids = {};  ///< PIDs of finished proc_table 
    queue rqueue = {};        ///< Ready queue of proc_table waiting to be scheduled
    map proc_table = {};      ///< Container for all proc_table
    Cores cores;              ///< Reference to the list of scheduler cores
    SchedulerConfig config;   ///< Runtime configuration settings

    // === Methods ===

    /** @brief Sets configuration and resizes cores. */
    void set_config(SchedulerConfig new_config) { config = move(new_config); }

    /** @brief Returns a unique, incrementing process ID. */
    uint new_pid() { return next_pid++; }

    /** @brief Adds a process to the process table. */
    void add_process(Process proc) {
      auto id = proc.data.id;
      auto ptr = make_unique<Process>(move(proc));
      proc_table.emplace(id, move(ptr)); 
    }

    /** @brief Check if a process with the given ID exists in the process table. */
    bool has_process(uint id) const { return proc_table.contains(id); }
      
    /** @brief Check if a process with the given name exists in the process table. */
    bool has_process(const str& name) const { return !!find_process_by_name(name, false); }

    // === Accessors ===
    
    /** @brief Returns a reference wrapper to the process with the given PID. */
    Process& get_process(uint id) { return *proc_table.at(id); }

    /** @brief Returns a reference wrapper to the process with the given name. */
    Process& get_process(const str& name) { return *find_process_by_name(name); }
    
    /** @brief Returns the list of running process IDs. */
    list get_running_pids() { return cores.get_running_pids(); }

    // ========================
    // === Private Helpers ====
    // ========================
    private:
  
    /** @brief Returns a pointer to a process given a name, or throws if not found (if enabed). */
    Process* find_process_by_name(const str& name, bool throw_if_missing=true) const {
      for (const auto& [_, proc]: proc_table)
        if (proc->data.name == name)
          return proc.get();
    
      if (throw_if_missing)
        throw runtime_error(format("Process with name '{}' not found.", name));
      return nullptr;
    }
  };   
}
