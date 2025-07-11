#pragma once
#include "core/common/utility/EventEmitter.hpp"
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"

namespace csopesy {

  /**
   * Represents a single CPU core in the scheduler system.
   * Each core may run one process job at a time and ticks independently in a thread.
   */
  class Core {
    public:
    using ProcessRef = ref<Process>;
    using ProcessCref = ref<const Process>;
    using Job = optional<ProcessRef>;
    using func = EventHandler::func;
    
    private:   
    static constexpr auto tick_delay = 1ms;
    
    uint id;                  ///< Core ID
    uint ticks;               ///< Ticks since the current job was assigned
    Job job;                  ///< Current assigned process (if any)
    Thread thread;            ///< Background ticking thread
    abool running = false;    ///< Atomic flag to keep the tick loop alive
    abool preempting = false; ///< Flag set to forcefully release the process
    EventEmitter emitter;     ///< Used for event hooks

    public:

    /** Constructs a scheduler core with the given ID and starts the thread. */
    Core(uint id=0): id(id) { start(); }

    /** Destructor stops the tick thread cleanly. */
    ~Core() { stop(); }

    /** Assigns a process to this core and updates its core ID. */
    void assign(ProcessRef ref) {
      job = move(ref);
      job->get().set_core(id);
      ticks = 0;
      
      emitter.emit("assign", any(job.value()));
      emitter.dispatch();
    }

    /** Returns the currently assigned job, or throws if none exists. */
    Process& get_job() {
      if (job) return job->get();
      throw runtime_error("Core::get_job: No job is currently assigned to this core");
    }

    /** Flags this core to preempt its current job. */
    void preempt() { preempting = true; }

    /** Returns the ID of the processor. */
    uint get_id() const { return id; }

    /** Returns true if the processor is currently idle (no job assigned). */
    bool is_idle() const { return !job.has_value(); }

    /** Returns the number of ticks the current job has run. */
    uint time_on_job() const { return ticks; }

    /** Registers an event listener for this core (e.g., "assign", "release"). */
    void on(const str& name, func handler) { emitter.on(name, handler); }

    private:

    /** Launches a background thread that ticks in a loop. */
    void start() {
      running = true;
      thread = Thread([this] {
        while (running) {
          tick();
          sleep_for(tick_delay);
        }
      });
    }
  
    /** Stops the tick loop and joins the thread. */
    void stop() {
      running = false;
      if (thread.joinable()) 
        thread.join();
    }

    /** Executes one instruction for the current job, and releases if done. */
    void tick() {
      if (!job) return;
 
      // Preemption has priority over stepping
      if (preempting)         
        return release(); 

      auto& process = job->get();
      process.step();
      ++ticks;
      
      // Finish after this tick
      auto& state = process.get_state();
      if (state.is_finished())  
        release();
    }

    // === Helpers ===

    /** Helper to release the current job and clears its core assignment. */
    void release() {
      emitter.emit("release", any(job.value()));
      emitter.dispatch();
      
      preempting = false;
      job->get().reset_core();
      job.reset();
      ticks = 0;
    }
  };
}
