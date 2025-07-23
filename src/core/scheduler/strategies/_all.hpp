#pragma once
#include "core/scheduler/SchedulerStrategy.hpp"
#include "fcfs.hpp"
#include "rr.hpp"


auto get_scheduler_strategy(const str& name) -> SchedulerStrategy {
  if (name == "fcfs") return make_fcfs_strategy();
  if (name == "rr")   return make_rr_strategy();
  throw runtime_error(format("Unknown strategy name: {}", name));
}
