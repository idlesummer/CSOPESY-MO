#include "core/common/imports/_all.hpp"

namespace csopesy::scheduler {
  using SchedulerData = str;      // placeholder aliases
  using SchedulerStrategy = str;  // placeholder aliases

  SchedulerStrategy make_fcfs_strategy() {
    auto handler = [](SchedulerData& data) {
      
      // for (auto& core: data.cores) {
      
      //   if (!core.is_idle())
      //     continue;

      //   while (!data.ready_queue.empty()) {
      //     auto job = data.ready_queue.front();
      //     data.ready_queue.pop();

      //     auto& process = job.get();

      //     if (process.get_state().is_finished())
      //       continue;

      //     if (process.get_core() != -1)
      //       continue;

      //     core.assign(job);
      //     break;
      //   }
      // }
    };

    // return SchedulerStrategy(handler);
  }
}
