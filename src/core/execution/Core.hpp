#pragma once
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
    using PreemptionPolicy = function<bool(const Core&)>;

    private:   
    static constexpr auto tick_delay = 1ms;
    
    uint id;                  ///< Core ID
    uint ticks = 0;           ///< Ticks since the current job was assigned
    Job job;                  ///< Currently assigned process (if any)
    Thread thread;            ///< Background ticking thread
    abool running = false;    ///< Atomic flag for tick loop
    PreemptionPolicy preempt_policy; ///< Optional strategy-injected logic
    bool should_release = false;     ///< Flag to indicate pending release

    public:

    /** Constructs a scheduler core with the given ID and starts the thread. */
    Core(uint id = 0) : id(id) { start(); }

    /** Destructor stops the tick thread cleanly. */
    ~Core() { stop(); }

    /** Assigns a process to this core and updates its core ID. */
    void assign(ProcessRef ref) {
      job = move(ref);
      job->get().set_core(id);
      ticks = 0;
      should_release = false;
    }

    /** Returns the currently assigned job. */
    Process& get_job() {
      if (job) return job->get();
      throw runtime_error("Core::get_job: No job is currently assigned");
    }

    /** Returns the core ID. */
    uint get_id() const { return id; }

    /** True if idle. */
    bool is_idle() const { return !job.has_value(); }

    /** True if this core wants to release the current job. */
    bool wants_release() const { return should_release; }

    /** Returns ticks since this job started. */
    uint ticks_on_job() const { return ticks; }

    /** Injects a preemption policy (optional). */
    void set_preemption_policy(PreemptionPolicy policy) {
      preempt_policy = policy;
    }

    /** Releases the current job (must be called manually by scheduler). */
    void release() {
      if (!job) return;
      job->get().reset_core();
      job.reset();
      ticks = 0;
      should_release = false;
    }

    private:

    /** Starts background thread. */
    void start() {
      running = true;
      thread = Thread([this] {
        while (running) {
          tick();
          sleep_for(tick_delay);
        }
      });
    }

    /** Stops thread cleanly. */
    void stop() {
      running = false;
      if (thread.joinable()) 
        thread.join();
    }

    /** Simulates one CPU tick. */
    void tick() {
      if (!job) return;

      auto& process = job->get();
      process.step();
      ++ticks;

      if (process.get_state().is_finished())
        should_release = true;

      if (preempt_policy && preempt_policy(*this))
        should_release = true;
    }
  };
}
