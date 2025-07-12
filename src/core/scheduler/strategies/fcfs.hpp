#include "core/common/imports/_all.hpp"
#include "core/scheduler/SchedulerData.hpp"
#include "core/scheduler/SchedulerStrategy.hpp"

namespace csopesy::scheduler {

/**
 * Standard, Lazy, Non-Preemptive, Immediate-Arrival FCFS strategy.
 *
 * Characteristics:
 * - Non-preemptive: process runs to completion once assigned
 * - Lazy: cores pull from the queue only when idle
 * - Immediate-arrival: processes are enqueued as soon as they arrive
 */
  inline SchedulerStrategy make_fcfs_strategy(const SchedulerConfig& config) {

    // Standard, Lazy, Non-Preemptive, Immediate-Arrival FCFS
    return SchedulerStrategy("fcfs")
      .with_config(config)

      // Each tick: assign next process to any idle core.
      .on_tick([](SchedulerData& data) {
        auto& rqueue = data.get_rqueue();

        // For every idle cores
        for (auto& ref: data.get_cores().get_idle()) {
          auto& core = ref.get();

          // If no process is ready, stop assigning
          if (rqueue.empty()) break;     

          // Assign the selected process to the current idle core
          uint pid = rqueue.front(); 
          rqueue.pop();
          core.assign(data.get_process(pid));
        }
    });
  }
}
