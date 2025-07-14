#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"
#include "core/execution/CoreManager.hpp"
#include "strategies/_all.hpp"
#include "SchedulerStrategy.hpp"
#include "SchedulerData.hpp"
#include "types.hpp"

namespace csopesy {

  /**
   * @brief Central scheduler controller for the OS simulation.
   * 
   * Owns the strategy, configuration, interpreter, and high-level
   * orchestration of ticks and process generation.
   */
  class Scheduler {
    using queue = vector<str>;
    using list = vector<uint>;

    queue names;                // Deferred generation queue for user-inserted proc_table
    
    public:

    // === Internal State ===
    uint ticks = 0;             // Global tick counter
    bool generating = false;    // Flag indicating auto-generation mode
    
    // === Components ===
    SchedulerData data;          // Internal state (cores, proc_table, queue)
    SchedulerStrategy strategy;  // Contains the scheduler strategy

    // === Methods ===

    /** @brief Executes the active strategy logic and increments the tick count. */
    void tick() {
      // 1. Generate any user-enqueued proc_table
      cout << "[tick] Stage 1: enqueue\n";
      for (auto& name: names)
        generate_process(move(name));
      names.clear();
      
      // 2. Possibly auto-generate proc_table this tick
      cout << "[tick] Stage 2: dummy\n";
      if (generating && interval_has_elapsed())
        generate_process();

      // 3. Update running/finished state and handle preemption
      cout << "[tick] Stage 3: core release\n";
      for (auto& ref: data.cores.get_busy()) {
        auto& core = ref.get();
        
        // Skip core if it's not releasable yet
        if (!core.can_release) continue;

        cout << "[tick]   core " << core.id << " releasing process\n";
        auto& process = core.get_job();
        core.release();

        if (process.data.state.finished()) {
          data.finished_pids.push_back(process.data.id);
          cout << "[tick]   process " << process.data.id << " finished\n";
        } else {
          data.rqueue.push(process.data.id);
          cout << "[tick]   process " << process.data.id << " re-queued\n";
        }
      }

      // 4. Schedule ready proc_table to idle cores
      cout << "[tick] Stage 4: strategy\n";
      // strategy.tick(data);
      ++ticks;
      cout << "[tick] End (tick " << ticks << ")\n";
    }

    /** @brief Applies a new configuration and resizes core state accordingly. */
    void set_config(SchedulerConfig config) {
      // 1. Build and install the selected strategy
      strategy = scheduler::make_strategy(config.scheduler, config);
      
      // 2. Inject per-core preemption policy from strategy
      if (strategy.preempt_handler) {
        for (auto& ref: data.cores.get_all())
          ref.get().set_preempt(strategy.preempt_handler);
      }
      
      // 3. Store config inside SchedulerData and initialize cores
      data.cores.resize(config.num_cpu);
      data.config = move(config);         // Move config last so it's not invalid above
    }

    // // === Process Generation Control ===
    // void enqueue_process(str name) { names.push_back(move(name)); }
    // void generate(bool flag) { generating = flag; }

    // ========================
    // === Private Helpers ====
    // ========================
    private:

    /** @brief Helper that checks if the current tick matches the process generation interval. */
    bool interval_has_elapsed() const {
      uint freq = data.config.batch_process_freq;
      return freq > 0 && (ticks % freq == 0);
    }

    /** @brief Internal helper that generates a process with optional name. */
    uint generate_process(str name="") { 
      // Generate a new unique process id
      uint pid = data.new_pid();
      
      // Create process in table so it's safe to reference
      data.add_process(move(Process::create(  
        pid,
        name.empty() ? format("p{:02}", pid) : name, 
        Random::num(data.config.min_ins, data.config.max_ins)
      )));

      // Enqueue the proces by its PID
      data.rqueue.push(pid);                   
      return pid;   
    }
  };
}
