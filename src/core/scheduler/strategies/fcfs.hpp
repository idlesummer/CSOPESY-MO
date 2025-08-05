#include "core/common/imports/_all.hpp"
#include "core/scheduler/SchedulerData.hpp"
#include "core/scheduler/SchedulerStrategy.hpp"


/**
 * Standard, Lazy, Non-Preemptive, Immediate-Arrival FCFS strategy.
 *
 * Characteristics:
 * - Non-preemptive: process runs to completion once assigned
 * - Lazy: cores pull from the queue only when idle
 * - Immediate-arrival: proc_table are enqueued as soon as they arrive
 */
auto make_fcfs_strategy() -> SchedulerStrategy {

  // Standard, Lazy, Non-Preemptive, Immediate-Arrival FCFS
  return SchedulerStrategy()
    .set_name("fcfs")

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
    });
}
