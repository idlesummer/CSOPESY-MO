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
 * - Immediate-arrival: proc_table are enqueued as soon as they arrive
 */
  inline SchedulerStrategy make_fcfs_strategy(const SchedulerConfig& config) {

    // Standard, Lazy, Non-Preemptive, Immediate-Arrival FCFS
    return SchedulerStrategy()
      .set_name("fcfs")
      .set_config(config)

      .on_tick([](SchedulerData& data) {;
        for (auto& ref: data.cores.get_idle()) {

          // If no process is ready, stop assigning
          if (data.rqueue.empty()) break;     
          
          // Assign the selected process to the current idle core
          auto& core = ref.get();
          uint pid = data.rqueue.front(); 
          data.rqueue.pop();
          core.assign(data.get_process(pid));
        }
    });
  }
}
