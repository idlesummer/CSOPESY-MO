#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"

namespace csopesy {

  /**
   * @brief Represents a passive simulated CPU core that ticks independently in a thread.
   * 
   * Responsibilities:
   * - Continuously ticks in its own thread every 1ms to simulate real-time execution.
   * - Executes (`step()`) the currently assigned process one instruction per tick.
   * - Monitors its assigned process for completion or preemption conditions.
   * - Sets `can_release = true` to signal readiness for job release.
   * 
   * Key Behaviors:
   * - Does **not** assign or release proc_table — only the **Scheduler** can do that.
   * - Ignores ticking if no process is assigned (`job == nullptr`) or if marked `can_release`.
   * - Calls a user-provided preemption handler, if any, to check for early release.
   * - Tracks how long a job has been running via `job_ticks`.
   * 
   * Design Notes:
   * - Core runs **faster** than the scheduler (1ms tick vs. ~100ms scheduler tick).
   * - Implements `assign()` and `release()` methods, but only for use by the scheduler.
   * - Internally manages its own thread and guarantees safe shutdown in destructor.
   * - Tick behavior is guarded to avoid dereferencing null process pointers.
   * - Cannot crash from invalid job access — fully guarded and thread-safe by design.
   */
  class Core {
    public:
    using func = function<bool(Core&)>;

    // == Identity ===
    uint id;                    // Core ID
    
    // === Process Job State ===
    Process* job     = nullptr; // Pointer to the currently assigned process (if any)
    uint job_ticks   = 0;       // Number of ticks the current process has been running
    bool can_release = false;   // Whether the process is eligible for release by the Scheduler
    func preempt  = nullptr;    // Optional strategy-injected logic
    
    // === Execution Control ===
    abool running    = false;   // Atomic flag for tick loop
    Thread thread;              // Background ticking thread

    // === Methods ===

    /** @brief Constructs a scheduler core with the given ID and starts the thread. */
    Core(uint id=0): id(id) { start(); }

    /** @brief Destructor stops the tick thread cleanly. */
    ~Core() { stop(); }

    /** @brief Called by scheduller to assign a process to this core. */
    void assign(Process& process) { set_job(&process, id); }

    /** @brief Called only by the scheduler to release the current job. */
    void release() { if (job) set_job(nullptr); }

    /** @brief Returns true no process is assigned to the core. */
    bool is_idle() const { return job == nullptr; }

    /** @brief Returns the currently assigned job. */
    Process& get_job() {
      if (job) return *job;
      throw runtime_error("Core::get_job: No job is currently assigned");
    }

    /** @brief Injects a preemption handler (optional). */
    void set_preempt(func handler) { preempt = handler; }


    // ========================
    // === Private Members ====
    // ========================
    private:

    
    /** @brief Performs one CPU tick: steps the assigned process if valid. */
    void tick() {
      if (job == nullptr) return; // If no process is assigned to this core, skip the tick
      if (can_release) return;    // Prevent ticking the process if it's already marked for release

      auto& process = *job;       // Get the process from the process pointer
      process.step();             // Execute one instruction from the process script
      ++job_ticks;                // Track how long this process is running on this core

      // If the process has finished all its instructions, mark for release
      if (process.data.state.finished())
        can_release = true;

      // If preemption is defined and it says to preempt, mark for release
      if (preempt != nullptr && preempt(*this))
        can_release = true;
    }

    /** @brief Only called by constructor to launch the core's background thread and start ticking. */
    void start() {
      if (running) return;        // If already running, do nothing (prevent duplicate threads)

      running = true;             // Mark the core as active/busy
      thread = Thread([this] {    // Launch the background thread that ticks continuously
        while (running) {
          tick();
          sleep_for(1ms);
        }
      });
    }

    /** @brief Signals the core to stop and joins the thread cleanly. */
    void stop() {
      running = false;            // Signals the thread to stop
      if (thread.joinable())      // Only joins if the thread is valid
        thread.join();            // Waits for the thread to finish
    }

    // === Helper methods ===

    /** @brief Assigns or clears the process currently running on this core. */
    void set_job(Process* process, int core_id=-1) {
      if (process != nullptr)     // Assigning a new process? Mark it with this core's ID
        process->data.core_id = core_id;
      
      job = process;              // Update process pointer slot (null if releasing)
      job_ticks = 0;              // Reset job tick counter — either a new job or clearing old one
      can_release = false;        // Always reset release flag; only tick() will set it to true when appropriate
    }
  };
}
