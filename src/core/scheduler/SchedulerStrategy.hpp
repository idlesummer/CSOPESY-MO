#pragma once
#include "core/common/imports/_all.hpp"
#include "core/execution/Core.hpp"
#include "core/scheduler/SchedulerData.hpp"

namespace csopesy {
  
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
    using TickHandler = function<void(SchedulerData&)>;

    // === Members ===
    str name;              // Strategy identifier (e.g., "fcfs", "rr")
    SchedulerConfig config;         // Strategy-specific configuration
    TickHandler tick_handler;       // Main strategy logic executed each tick
    PreemptHandler preempt_handler; // Core-level preemption policy (optional)

    // === Methods Methods ===

    /** @brief Sets the strategy name. */
    SchedulerStrategy& set_name(str n) { return name = n, *this; }

    /** @brief Sets the configuration for this strategy. */
    SchedulerStrategy& set_config(SchedulerConfig c) { return config = move(c), *this; }

    /** @brief Sets the main logic to run on each tick. */
    SchedulerStrategy& on_tick(TickHandler t) { return tick_handler = move(t), *this; }

    /** @brief Sets the per-core preemption policy. */
    SchedulerStrategy& on_preempt(PreemptHandler p) { return preempt_handler = move(p), *this; }

    // === Execution ===

    /** @brief Invokes the tick handler logic on the current scheduler data. */
    void tick(SchedulerData& data) {
      if (!tick_handler)
        throw runtime_error("SchedulerStrategy::tick called without on_tick handler.");
      tick_handler(data);
    }
  };
}
