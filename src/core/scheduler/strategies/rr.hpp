#pragma once
#include "core/common/imports/_all.hpp"
#include "core/execution/Core.hpp"
#include "core/scheduler/SchedulerData.hpp"
#include "core/scheduler/SchedulerStrategy.hpp"


/**
 * Round-Robin Strategy (preemptive).
 * 
 * Characteristics:
 * - Preemptive: proc_table are interrupted after a time quantum.
 * - Immediate arrival: proc_table are placed into the ready queue directly.
 */
auto make_rr_strategy() -> SchedulerStrategy {
  return SchedulerStrategy()
    .set_name("rr")

    .on_tick([](SchedulerData& data) {
      for (auto& ref : data.cores.get_idle()) {
        // No processes to assign
        if (data.rqueue.empty()) 
          break;

        // Peek at the front process
        auto pid = data.rqueue.front();

        // Check if there's enough memory to run this process
        // Not enough memory — skip for now, leave in rqueue
        // Pass true if process only needs at least 1 free frame
        // otherwise require full memory allocation
        if (!data.memory_available_for(pid, true))
          continue;

        // Enough memory - assign to core
        data.rqueue.pop();
        auto& core = ref.get();
        core.assign(data.get_process(pid));
      }
    })

    .on_preempt([](SchedulerData& data) -> Core::func {
      auto quantum = data.config.getu("quantum-cycles");
      return [quantum](Core& core) -> bool {
        return core.job_ticks >= quantum;
      };
    });
}
