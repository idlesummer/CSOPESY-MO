#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"


namespace csopesy {

  /**
   * Represents a single CPU core in the scheduler system.
   * Each core may run one job (process) at a time and ticks independently in a thread.
   */
  class SchedulerCore {
    public:
    using ProcessRef = ref<Process>;
    using Job = optional<ProcessRef>;
    
    private:
    Job job;
    uint id;
    Thread thread;
    abool running = false;

    public:

    /** Constructs a scheduler core with the given ID and starts the thread. */
    SchedulerCore(uint id=0): id(id) { start(); }
    ~SchedulerCore() { stop(); }

    /** Assigns a process to this core and updates its core ID. */
    void assign(ProcessRef procref) {
      job = move(procref);
      auto& process = procref.get();
      process.set_core(id);
    }

    /** Releases the current job and clears its core assignment. */
    void release() {
      if (!job) return;

      auto& process = job->get();
      process.reset_core();
      job.reset();
    }

    // === Accessors ===

    /** Returns the currently assigned job, or throws if none exists. */
    Process& get_job() {
      if (job) return job->get();
      throw runtime_error("No job assigned to core");
    }

    uint id() const { return id; }
    bool is_idle() const { return !job.has_value(); }
    bool is_running() const { return job.has_value(); }

    // === Lifecycle methods ===

    /** Launches a background thread that ticks in a loop. */
    void start() {
      if (running) return;
      
      running = true;
      thread = Thread([&] {
        while (running) {
          tick();
          sleep_for(1ms);
        }
      });
    }

    /** Stops the tick loop and joins the thread. */
    void stop() {
      running = false;
      if (thread.joinable()) thread.join();
    }

    private:

    /** Executes one instruction for the current job, and releases if done. */
    void tick() {
      if (!job) return;     
      if (auto& process = job->get(); process.step())
        release();
    }
  };
}
