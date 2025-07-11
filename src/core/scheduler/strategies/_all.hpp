#pragma once
#include "core/common/imports/_all.hpp"
#include "core/scheduler/SchedulerStrategy.hpp"
#include "fcfs.hpp"

namespace csopesy::scheduler {
  using list = vector<SchedulerStrategy>;
  
  list get_all() {
    return {
      make_fcfs_strategy(),
      // make_rr_strategy(),
    };
  }
}
