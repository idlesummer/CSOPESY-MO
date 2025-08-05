#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"
#include "core/execution/CoreManager.hpp"
#include "core/memory/MemoryManager.hpp"
#include "types.hpp"


/**
 * @brief Encapsulates scheduling context passed to strategy functions.
 * 
 * Provides access to the current core vec<uint>, ready queue, and tick counter.
 * Also handles state reset and tick progression during simulation.
 */
class SchedulerData {  
  public:
  
  SchedulerData():
    proc_table    (umap<uint,uptr<Process>>()), // Container for all processes
    finished_pids (vec<uint>()),                // PIDs of Finished processes 
    rqueue        (queue<uint>()),              // Ready queue of processes waiting to be scheduled
    wqueue        (list<uint>()),               // Ready queue of processes waiting to be scheduled
    next_pid      (atomic_uint{1}),             // PID counter for generating unique process IDs
    config        (Config()),                   // Runtime configuration settings
    cores         (CoreManager()),              // Owned instance of core manager
    memory        (MemoryManager()) {}          // Owned instance of memory manager

  /** @brief Returns a unique, incrementing process ID. */
  auto new_pid() -> uint { return next_pid++; }

  /** @brief Adds a process to the process table. */
  void add_process(Process proc) {
    auto id = proc.data.id;
    auto ptr = make_unique<Process>(move(proc));
    proc_table.emplace(id, move(ptr)); 
  }

  /** @brief Check if a process with the given ID exists. */
  auto has_process(uint id) -> bool { return proc_table.contains(id); }

  /** @brief Check if a process with the given name exists. */
  auto has_process(const str& name) -> bool { return !!find_process_by_name(name, false); }
  
  /** @brief Returns a reference wrapper to the process with the given PID. */
  auto get_process(uint id) -> Process& { return *proc_table.at(id); }

  /** @brief Returns a reference wrapper to the process with the given name. */
  auto get_process(const str& name) -> Process& { return *find_process_by_name(name); }
  
  /** @brief Returns the vec<uint> of running process IDs. */
  auto get_running_pids() -> vec<uint> { return cores.get_running_pids(); }

  /**
   * @brief Determines if a process can be scheduled based on current memory state.
   *
   * A process is eligible to run if:
   * - At least one of its pages is already loaded, OR
   * - At least one free frame exists to satisfy future faults.
   */
  auto memory_available_for(uint pid) -> bool {
    
    // cout << format("[debug] Pid: {}\n", pid);
    // cout << "[debug] Page Table Map Contents:\n";
    // for (auto& [pid, pt] : memory.data.page_table_map) {
    //   cout << format("  PID {}: {} pages\n", pid, pt.pages().size());
    //   for (auto page_num : pt.pages()) {
    //     auto& entry = pt.get(page_num);
    //     cout << format("    Page {} → {}\n", page_num, 
    //       entry.is_loaded() ? format("frame {}", entry.frame()) : "not loaded");
    //   }
    // } 

    // Check if the process has pages
    if (!memory.data.page_table_map.contains(pid))
      return false;
    
    auto& page_table = memory.data.page_table_map.at(pid);
    // cout << format("[debug] PID {} has {} pages and {} free frames\n", pid, page_table.pages().size(), memory.data.free_frames.size());

    // If any of its pages are loaded, it's good to run
    for (auto page_num : page_table.pages())
      if (page_table.get(page_num).is_loaded())
        return true;

    // Otherwise, check if there's at least one free frame
    return !memory.data.free_frames.empty();
  }

  // ------ Instance variables ------
  umap<uint, uptr<Process>> proc_table;
  vec<uint> finished_pids;         
  queue<uint> rqueue;                 
  list<uint> wqueue;                 
  atomic_uint next_pid;           
  Config config;
  CoreManager cores;
  MemoryManager memory;

  // ------ Internal logic ------
  private:

  /** @brief Returns a pointer to a process given a name, or throws if not found (if enabed). */
  auto find_process_by_name(const str& name, bool throw_if_missing=true) -> Process* {
    for (auto& [_, proc]: proc_table)
      if (proc->data.name == name)
        return proc.get();
  
    if (throw_if_missing)
      throw runtime_error(format("Process with name '{}' not found.", name));
    return nullptr;
  }
};
