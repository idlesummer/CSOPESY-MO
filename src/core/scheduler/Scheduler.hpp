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
    names      (vec<str>()),            // Deferred generation vec<str> for user-inserted proc_table
    ticks      (0),                    // Global tick counter
    generating (false),                 // Flag indicating auto-generation mode
    data       (SchedulerData()),       // Internal state (cores, proc_table, vec<str>)
    strategy   (SchedulerStrategy()) {} // Contains the scheduler strategy

  /** Adds a user-named process to the pending generation vec<str>. */
  void enqueue_process(str name) { names.push_back(move(name)); }

  /** Enables or disables automatic process generation each tick. */
  void generate(bool flag) { generating = flag; }

  /** @brief Executes the active strategy logic and increments the tick count. */
  void tick() {
    if (!data.config.initialized) return;

    // Generate batch processes
    generate_processes();

    // Release finished or preempted cores
    release_processes();

    // TODO: Clean/update paging info  
    // memory.tick(data);

    // Assign new processes to idle cores
    strategy.tick(data);
    ++ticks;
  }

  /** @brief Applies a new configuration and resizes core state accordingly. */
  void set_config(SchedulerConfig config) {
    strategy = get_scheduler_strategy(config.scheduler, config); 
    data.cores.resize(config.num_cpu);                            
    
    // Inject in each core the preemption handler from strategy
    if (strategy.preempt_handler)         
      for (auto& ref: data.cores.get_all())                       
        ref.get().set_preempt(strategy.preempt_handler);

    data.config = move(config);                                   
  }

  // ------ Member variables ------

  vec<str> names;                
  uint ticks;             
  bool generating;    
  SchedulerData data;         
  SchedulerStrategy strategy;

  // ------ Internal logic ------

  private:

  /** @brief Helper that checks if the current tick matches the process generation interval. */
  auto interval_has_elapsed() -> bool {
    uint freq = data.config.batch_process_freq;
    return freq > 0 && (ticks % freq == 0);
  }

  /** @brief Helper that generates user and scheduler-enqueued processes. */
  void generate_processes() {
    auto make_process = [&](uint pid, str name="") {
      auto pname = name.empty() ? format("p{:02}", pid) : move(name);
      auto size  = Rand::num(data.config.min_ins, data.config.max_ins);
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
      else
        data.rqueue.push(process.data.id);
    }
  }
};
