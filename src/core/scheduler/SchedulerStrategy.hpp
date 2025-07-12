#pragma once
#include "core/common/imports/_all.hpp"
#include "core/execution/Core.hpp"
#include "core/scheduler/SchedulerData.hpp"

namespace csopesy {
  
  /**
   * @brief Represents a pluggable scheduling strategy (e.g., FCFS, RR).
   * 
   * Delegates scheduling logic and per-core preemption logic.
   */
  class SchedulerStrategy {
    public:
    using Job = Core::Job;
    using TickHandler = function<void(SchedulerData&)>;
    using PreemptPolicy = function<bool(const Core&)>;

    private:
    SchedulerConfig config;
    str name;
    TickHandler handle_tick;
    PreemptPolicy handle_preempt;

    public:
    SchedulerStrategy(str name): name(name) {

    }

    // === Metadata ===

    const str& get_name() const { return name; }

    // === Configuration ===

    SchedulerStrategy& with_config(SchedulerConfig cfg) {
      config = move(cfg);
      return *this;
    }

    SchedulerStrategy& on_tick(TickHandler handler) {
      handle_tick = move(handler);
      return *this;
    }

    SchedulerStrategy& on_preempt(PreemptPolicy policy) {
      handle_preempt = move(policy);
      return *this;
    }

    // === Accessors ===

    const PreemptPolicy& get_preemption_policy() const {
      return handle_preempt;
    }

    const SchedulerConfig& get_config() const {
      return config;
    }

    // === Execution ===

    void tick(SchedulerData& data) {
      if (!handle_tick)
        throw runtime_error("SchedulerStrategy::tick called without on_tick handler.");
      handle_tick(data);
    }
  };
}
