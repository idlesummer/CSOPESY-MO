#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"
#include "core/scheduler/SchedulerCore.hpp"

namespace csopesy {
  struct SchedulerData {
    using list = vector<SchedulerCore>;
    using queue = queue<ref<Process>>;

    list& cores;
    queue& ready_queue;
    uint ticks;

    SchedulerData(list& cores, queue& ready_queue, uint ticks): 
      cores(cores), ready_queue(ready_queue), ticks(ticks) {}
  };
}
