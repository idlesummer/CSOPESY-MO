#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"
#include "core/execution/CoreManager.hpp"
#include "strategies/_all.hpp"
#include "SchedulerStrategy.hpp"
#include "SchedulerData.hpp"
#include "types.hpp"


/**
 * @brief Central scheduler controller for the OS simulation.
 * 
 * Owns the strategy, configuration, interpreter, and high-level
 * orchestration of ticks and process generation.
 */
class Scheduler {
  public:

  Scheduler():
    names      (vec<str>()),            // Deferred generation list for user-inserted process table
    ticks      (0u),                    // Global tick counter
    generating (false),                 // Flag indicating auto-generation mode
    data       (SchedulerData()),       // Internal state (cores, process table, vec<str>)
    strategy   (SchedulerStrategy()) {} // Contains the scheduler strategy

  /** Adds a user-named process to the pending generation vec<str>. */
  void generate_process(str name) { names.push_back(move(name)); }

  /** Enables or disables automatic process generation each tick. */
  void generate(bool flag) { generating = flag; }

  /** @brief Executes the active strategy logic and increments the tick count. */
  void tick() {
    if (!data.config.getb("initialized")) return;

    try {
      // Generate batch processes
      generate_processes();

      // Release finished or preempted cores
      release_processes();
      
      /** TODO: Clean/update paging info. */ 
      // memory.tick(data);
      
      // Tick sleeping processes in the waiting queue
      tick_sleeping_processes();

      // Assign new processes to idle cores
      strategy.tick(data);
      ++ticks;

    } catch (exception& e) {
      cerr << format("[Scheduler] tick(): Exception: {}\n", e.what());
      throw;
    }
  }

  /** @brief Applies a new configuration and resizes core state accordingly. */
  void set_config(Config config) {
    strategy = get_scheduler_strategy(config.gets("scheduler"));
    data.cores.resize(config.getu("num-cpu"));

    // Create the preempt handler from the factory method
    auto preempt_handler = strategy.get_preempt_handler(data);

    // Inject in each core the preemption handler from strategy
    if (preempt_handler != nullptr) {
      for (auto& ref: data.cores.get_all()) {
        auto& core = ref.get();

        // Configure cores with the tick delay and the preeption handler
        core.delay = config.getu("delays-per-exec");
        core.preempt = preempt_handler;
      }
    }

    data.config = move(config); // Must come last                               
  }

  // ------ Instance variables ------

  vec<str> names;                
  uint ticks;             
  bool generating;    
  SchedulerData data;         
  SchedulerStrategy strategy;

  // ------ Internal logic ------

  private:

  /** @brief Helper that checks if the current tick matches the process generation interval. */
  auto interval_has_elapsed() -> bool {
    uint freq = data.config.getu("batch-process-freq");
    return freq > 0 && (ticks % freq == 0);
  }

  /** @brief Helper that generates user and scheduler-enqueued processes. */
  void generate_processes() {
    auto make_process = [&](uint pid, str name="") {
      auto pname = name.empty() ? format("p{:02}", pid) : move(name);
      auto min = data.config.getu("min-ins");
      auto max = data.config.getu("max-ins");
      auto size = Rand::num(min, max);

      data.add_process(Process(pid, move(pname), size));
      data.rqueue.push(pid);
    };

    for (auto& name: names)
      make_process(data.new_pid(), move(name));
    names.clear();

    if (generating && interval_has_elapsed())
      make_process(data.new_pid());
  }

  /** @brief Helper that releases finished or preempted processes in cores. */
  void release_processes() {
    for (auto& ref: data.cores.get_releasable()) {
      auto& core = ref.get();
      auto& process = core.get_job();
      core.release();

      if (process.data.program.finished())
        data.finished_pids.push_back(process.data.id);
      else if (process.data.control.sleeping())
        data.wqueue.push_back(process.data.id);     
      else                                      
        data.rqueue.push(process.data.id);  // Not finished, not sleeping → just resume later
    }
  }

  /** @brief Steps processes in wqueue. Returns processes to the ready queue once they're not sleeping. */
  void tick_sleeping_processes() {
    auto& wqueue = data.wqueue;
    for (auto it = wqueue.begin(); it != wqueue.end(); ) {
      auto& process = data.get_process(*it);
      process.step();           // decrement sleep_ticks

      if (!process.data.control.sleeping()) {
        data.rqueue.push(*it);  // ready again
        it = wqueue.erase(it);  // remove from wqueue
        continue;
      }

      ++it;                     // still sleeping
    }
  }
};
