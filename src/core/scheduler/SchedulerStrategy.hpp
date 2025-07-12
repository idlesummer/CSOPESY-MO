#pragma once
#include "core/common/imports/_all.hpp"
#include "core/execution/Core.hpp"
#include "core/scheduler/SchedulerData.hpp"

namespace csopesy {
  
  /**
   * @brief Represents a pluggable scheduling strategy (e.g., FCFS, RR).
   * 
   * Allows dynamic assignment of tick behavior and preemption policy.
   * Each strategy defines how processes are selected for execution,
   * and whether/when a running process should be preempted.
   */
  class SchedulerStrategy {
    public:
    using Job = Core::Job;
    using PreemptHandler = Core::Handler;
    using TickHandler = function<void(SchedulerData&)>;

    private:
    str name;                       // Strategy identifier (e.g., "fcfs", "rr")
    TickHandler handle_tick;        // Main strategy logic executed each tick
    PreemptHandler handle_preempt;  // Core-level preemption policy (optional)
    SchedulerConfig config;         // Strategy-specific configuration

    public:

    /** @brief Constructs a strategy with the given name. */
    SchedulerStrategy(str name): name(move(name)) {}

    /** @brief Returns the name of the strategy. */
    const str& get_name() const { return name; }

    // === Configuration ===

    /** @brief Sets the configuration for this strategy. */
    SchedulerStrategy& with_config(SchedulerConfig cfg) {
      config = move(cfg);
      return *this;
    }

    /** @brief Sets the main logic to run on each tick. */
    SchedulerStrategy& on_tick(TickHandler handler) {
      return handle_tick = move(handler), *this;
    }

    /** @brief Sets the per-core preemption policy. */
    SchedulerStrategy& on_preempt(PreemptHandler policy) {
      return handle_preempt = move(policy), *this;
    }

    // === Accessors ===

    /** @brief Returns the scheduler configuration. */
    const SchedulerConfig& get_config() const { return config; }

    /** @brief Returns the core preemption policy handler. */
    const PreemptHandler& get_prempt() const { return handle_preempt; }

    // === Execution ===

    /** @brief Invokes the tick handler logic on the current scheduler data. */
    void tick(SchedulerData& data) {
      if (!handle_tick)
        throw runtime_error("SchedulerStrategy::tick called without on_tick handler.");
      handle_tick(data);
    }
  };
}
