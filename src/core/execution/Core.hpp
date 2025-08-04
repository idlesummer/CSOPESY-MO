#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"


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
  using func = func<bool(Core&)>;

  /** @brief Constructs a core with the given ID and starts the thread. */
  Core(uint id=0): 
    id          (id),                 // Core ID
    job_ticks   (0u),                 // Number of ticks the current process has been running
    delay       (0u),                 // Target delay per instruction (delay_per_exec)
    can_release (false),              // Whether the process is eligible for release by the Scheduler
    job         (nullptr),            // Pointer to the currently assigned process (if any)
    preempt     (nullptr),            // For injecting a preemption handler (optional).
    active      (atomic_bool{true}),  // Atomic flag for tick loop, mark the core as active/busy
    thread      ()                    // Background ticking thread
  { 
    // Launch the background thread that ticks continuously
    thread = Thread([this] { 
      auto counter = 0u;

      while (active) {
        if (counter-- == 0) {
          with_locked([&] { tick(); });
          counter = delay;
        }
        sleep_for(1ms);  // always happens
      }
    });
  }

  /** @brief Destructor stops the tick thread cleanly. */
  ~Core() { stop(); }

  /** @brief Initializes core with a given delay and optional preemp handler. */
  void init(uint ticks, func handler=nullptr) {
    delay = ticks;
    preempt = handler;
  }

  /** @brief Called by scheduller to assign a process to this core. */
  void assign(Process& process) { set_job(&process, id); }

  /** @brief Called only by the scheduler to release the current job. */
  void release() { if (job) set_job(nullptr); }

  /** @brief Returns true no process is assigned to the core. */
  auto is_idle() -> bool { return job == nullptr; }

  /** @brief Returns the currently assigned job. */
  auto get_job() -> Process& {
    if (job) return *job;
    throw runtime_error("Core::get_job: No job is currently assigned");
  } 

  // ------ Instance variables ------

  uint id;                      
  uint job_ticks;       
  uint delay;       
  bool can_release;   
  Process* job; 
  func preempt;    
  atomic_bool active;   
  Thread thread; 

  // ------ Internal logic ------

  private:

  /** @brief Performs one CPU tick: steps the assigned process if valid. */
  void tick() {
    if (job == nullptr) return; // If no process is assigned to this core, skip the tick
    if (can_release) return;    // Prevent ticking the process if it's already marked for release

    try {
      auto& process = *job;       // Get the process from the process pointer
      process.step();             // Execute one instruction from the process script
      ++job_ticks;

      // If the process has finished all its instructions, mark for release
      if (process.data.program.finished())
        can_release = true;

      // Mark release if process is sleeping
      else if (process.data.control.sleeping()) // COMMENT OUT IF SLEEPING DOESN'T PREEMPT A PROCESS!!!
        can_release = true;

      // Release if the preemption logic says so
      else if (preempt && preempt(*this))
        can_release = true;

    } catch (exception& e) {
      cerr << format("[Core {}] tick(): Exception: {}\n", id, e.what());
      throw;
    }
  }

  /** @brief Signals the core to stop and joins the thread cleanly. */
  void stop() {
    active = false;             // Signals the thread to stop
    if (thread.joinable())      // Only joins if the thread is valid
      thread.join();            // Waits for the thread to finish
  }

  // ------ Helper methods ------

  /** @brief Assigns or clears the process currently running on this core. */
  void set_job(Process* process, int core_id=-1) {
    if (process != nullptr)     // Assigning a new process? Mark it with this core's ID
      process->data.core_id = core_id;
    
    job = process;              // Update process pointer slot (null if releasing)
    job_ticks = 0;              // Reset job tick counter — either a new job or clearing old one
    can_release = false;        // Always reset release flag; only tick() will set it to true when appropriate
  }    
};
