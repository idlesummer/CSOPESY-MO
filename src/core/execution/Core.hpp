#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"

namespace csopesy {

  /**
   * Represents a single CPU core in the scheduler system.
   * Each core may run one process job at a time and job_ticks independently in a thread.
   */
  class Core {
    public:
    using Job = Process*;       /// Job just means a process pointer
    using Handler = function<bool(const Core&)>;

    public:  
    uint id;                    ///< Core ID
    
    // === Job State ===
    Job job          = nullptr; ///< A 'slot' for the currently assigned process (if any)
    uint job_ticks   = 0;       ///< Ticks since the current job was assigned
    bool can_release = false;   ///< Flag to indicate pending release
    Handler preempt  = nullptr; ///< Optional strategy-injected logic
    
    // === Execution Control ===
    abool running    = false;   ///< Atomic flag for tick loop
    Thread thread;              ///< Background ticking thread

    // === Methods ===

    /** @brief Constructs a scheduler core with the given ID and starts the thread. */
    Core(uint id=0): id(id) { 
      start(); 
    }

    /** @brief Destructor stops the tick thread cleanly. */
    ~Core() { stop(); }

    /** @brief Called by scheduller to assign a process to this core. */
    void assign(Process& process) { set_job(&process, id); }

    /** @brief Called by scheduller to release the current job. */
    void release() { if (job) set_job(nullptr); }

    /** @brief True if idle. */
    bool is_idle() const { return job == nullptr; }

    /** @brief Returns the currently assigned job. */
    Process& get_job() {
      if (job) return *job;
      throw runtime_error("Core::get_job: No job is currently assigned");
    }

    /** @brief Injects a preemption handler (optional). */
    void set_preempt(Handler handler) { preempt = handler; }


    private:


    /** @brief Simulates one CPU tick. */
    void tick() {
      if (job == nullptr) return;   // If no process is assigned to this core, skip the tick
      if (can_release) return;      // Prevent ticking the process if it's already marked for release

      auto& process = *job;         // Get the process from the process pointer
      process.step();               // Execute one instruction from the process script
      ++job_ticks;                  // Track how long this process is running on this core

      // If the process has finished all its instructions, mark for release
      if (process.data.state.finished())
        can_release = true;

      // If preemption is defined and it says to preempt, mark for release
      if (preempt != nullptr && preempt(*this))
        can_release = true;
    }

    /** @brief Starts background thread. */
    void start() {
      cout << format("[Core {}]: ENTERING start()\n", id) << flush;
      if (running) {
        cout << format("[Core {}] WARNING: start() called while already running!\n", id);
        return;
      }

      running = true;
      cout << "[Core X] before thread\n" << flush;
      thread = Thread([this] {
        while (running) {
          tick();
          sleep_for(1ms);
        }
      });
      cout << "[Core X] after thread\n" << flush;
    }

    /** @brief Stops thread cleanly. */
    void stop() {
      running = false;
      if (thread.joinable()) 
        thread.join();
    }

    // === Helper methods ===

    void set_job(Job new_job, int core_id=-1) {

      // If we're assigning a new process, set the id of the core owns it
      if (new_job != nullptr)
        new_job->data.core_id = core_id;

      // Update process pointer slot (null if releasing)
      job = new_job;

      // Reset job tick counter — either a new job or clearing old one
      job_ticks = 0;

      // This process pointer is not releasable yet — wait for tick() to decide that
      can_release = false;
    }
  };
}
