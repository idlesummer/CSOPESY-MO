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

        // cout << "[Tick] Rqueue front-to-back: ";
        // auto& running = data.get_running();
        // auto rqueue_copy = data.get_rqueue();

        // while (!rqueue_copy.empty()) {
        //   cout << rqueue_copy.front() << " ";
        //   rqueue_copy.pop();
        // }
        // cout << endl;

        auto& rqueue = data.get_rqueue();

        for (auto& ref: data.get_cores().get_idle()) {
          auto& core = ref.get();

          if (rqueue.empty()) break;

          uint pid = rqueue.front(); 
          rqueue.pop();

          core.assign(data.get_process(pid));
        }
      })
      
      .on_preempt([quantum = config.quantum_cycles](const Core& core) {
        return core.ticks_on_job() >= quantum;  
      });
  }
}
