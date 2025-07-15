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
   * - Preemptive: proc_table are interrupted after a time quantum.
   * - Immediate arrival: proc_table are placed into the ready queue directly.
   */
  inline SchedulerStrategy make_rr_strategy(const SchedulerConfig& config) {
    return SchedulerStrategy()
      .set_name("rr")
      .set_config(config)
      
      .on_tick([](SchedulerData& data) {
        for (auto& ref: data.cores.get_idle()) {
      
          // If no process is ready, stop assigning
          if (data.rqueue.empty()) break;
          
          auto& core = ref.get();
          uint pid = data.rqueue.front(); 
          data.rqueue.pop();
          core.assign(data.get_process(pid));
        }
      })

      .on_preempt([quantum=config.quantum_cycles](Core& core) {
        return core.job_ticks >= quantum;  
      });
  }
}
