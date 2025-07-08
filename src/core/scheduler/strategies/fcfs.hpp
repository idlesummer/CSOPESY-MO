#include "core/common/imports/_all.hpp"
#include "core/scheduler/SchedulerData.hpp"
#include "core/scheduler/SchedulerStrategy.hpp"

namespace csopesy::scheduler {
  inline SchedulerStrategy make_fcfs_strategy() {
    using Strategy = SchedulerStrategy;
    using Job = Strategy::Job;
    using SelectHandler = Strategy::SelectHandler;

    return Strategy()
      
      // When a process is added, push it to the ready queue.
      .on_add([](Process& process, SchedulerData& data) {
        auto& rqueue = data.get_rqueue();
        rqueue.push(ref(process));
      })
      
      // Select the next process by popping from the front of the queue.
      .on_select([](SchedulerData& data) -> Job {
        auto& rqueue = data.get_rqueue();
        if (rqueue.empty()) 
          return nullopt;

        auto process = rqueue.front();
        rqueue.pop();
        return process;
      })

      // Each tick: assign next process to any idle core.
      .on_tick([](SchedulerData& data, const SelectHandler& select) {
        for (auto& core: data.get_cores()) {
          if (!core.is_idle()) continue;       
          if (auto job = select(data))
            core.assign(job.value());
        }
      });
  }
}
