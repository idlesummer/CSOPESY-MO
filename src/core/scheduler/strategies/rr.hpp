#pragma once
#include "core/common/imports/_all.hpp"
#include "core/execution/Core.hpp"
#include "core/scheduler/SchedulerData.hpp"
#include "core/scheduler/SchedulerStrategy.hpp"

namespace csopesy::scheduler {

  /**
   * Round-Robin Strategy (preemptive).
   * 
   * Characteristics:
   * - Preemptive: processes are interrupted after a time quantum.
   * - Immediate arrival: processes are placed into the ready queue directly.
   */
  inline SchedulerStrategy make_rr_strategy(const SchedulerConfig& config) {
    return SchedulerStrategy("rr")
      .with_config(config)

      // Tick: assign ready jobs to idle cores
      .on_tick([](SchedulerData& data) {
        for (auto& ref: data.cores.get_idle()) {
          auto& core = ref.get();

          if (data.rqueue.empty()) break;

          uint pid = data.rqueue.front(); 
          data.rqueue.pop();

          core.assign(data.get_process(pid));
        }
      })

      .on_preempt([quantum = config.quantum_cycles](const Core& core) {
        return core.job_ticks >= quantum;  
      });
  }
}
