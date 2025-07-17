#pragma once
#include "core/common/imports/_all.hpp"
#include "core/execution/Core.hpp"
#include "core/scheduler/SchedulerData.hpp"

  
/**
 * @brief Represents a pluggable scheduling strategy (e.g., FCFS, RR).
 * 
 * Allows dynamic assignment of tick behavior and preemption policy.
 * Each strategy defines how proc_table are selected for execution,
 * and whether/when a running process should be preempted.
 */
class SchedulerStrategy {
  public:

  using PreemptHandler = Core::func;
  using TickHandler = func<void(SchedulerData&)>;

  SchedulerStrategy():
    name            (""s),                // Strategy identifier (e.g., "fcfs", "rr")
    config          (SchedulerConfig()),  // Strategy-specific configuration
    tick_handler    (nullptr),            // Main strategy logic executed each tick
    preempt_handler (nullptr) {}          // Core-level preemption policy (optional)


  /** @brief Sets the strategy name. */
  auto set_name(str n) -> SchedulerStrategy& { return name = n, *this; }

  /** @brief Sets the configuration for this strategy. */
  auto set_config(SchedulerConfig c) -> SchedulerStrategy& { return config = move(c), *this; }

  /** @brief Sets the main logic to run on each tick. */
  auto on_tick(TickHandler t) -> SchedulerStrategy& { return tick_handler = move(t), *this; }

  /** @brief Sets the per-core preemption policy. */
  auto on_preempt(PreemptHandler p) -> SchedulerStrategy& { return preempt_handler = move(p), *this; }

  // === Execution ===

  /** @brief Invokes the tick handler logic on the current scheduler data. */
  void tick(SchedulerData& data) {
    if (!tick_handler)
      throw runtime_error("SchedulerStrategy::tick called without on_tick handler.");
    tick_handler(data);
  }

  // ------ Member variables ------

  str name;              
  SchedulerConfig config;         
  TickHandler tick_handler;       
  PreemptHandler preempt_handler;
};
