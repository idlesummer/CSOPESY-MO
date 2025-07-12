#pragma once
#include "core/scheduler/SchedulerStrategy.hpp"
#include "core/scheduler/types.hpp"
#include "fcfs.hpp"
#include "rr.hpp"

namespace csopesy::scheduler {

  inline SchedulerStrategy make_strategy(const str& name, const SchedulerConfig& config) {
    if (name == "fcfs") return make_fcfs_strategy(config);
    if (name == "rr")   return make_rr_strategy(config);
    throw runtime_error(format("Unknown strategy name: {}", name));
  }
}
