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
  inline SchedulerStrategy make_fcfs_strategy() {
    using SelectHandler = SchedulerStrategy::SelectHandler;
    using Job = SchedulerStrategy::Job;

    // Standard, Lazy, Non-Preemptive, Immediate-Arrival FCFS
    return SchedulerStrategy("fcfs")

      // When a process is added, push it to the ready queue.
      .on_add([](Process& process, SchedulerData& data) {
        auto& rqueue = data.get_rqueue();
        rqueue.push(ref(process));
      })

      // Select the next process by popping from the front of the queue.
      .on_select([](SchedulerData& data) -> Job {
        auto& rqueue = data.get_rqueue();
        if (rqueue.empty()) return nullopt;

        auto process = rqueue.front();
        rqueue.pop();
        return process;
      })

      // Each tick: assign next process to any idle core.
      .on_tick([](SchedulerData& data, const SelectHandler& select) {
        // For every idle cores
        for (auto& ref: data.get_cores().get_idle()) {
          auto& core = ref.get();
          
          // Select the next proces from the rqueue
          auto job = select(data);  

          // If no process is ready, stop assigning
          if (!job) break;        

          // Assign the selected process to the current idle core
          core.assign(job.value());
        }
    });
  }
}
