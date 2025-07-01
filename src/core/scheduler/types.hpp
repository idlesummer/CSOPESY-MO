#pragma once
#include "core/common/imports/_all.hpp"

namespace csopesy {
  struct SchedulerConfig {
    uint num_cpu = 1;
    str  scheduler = "fcfs";
    uint quantum_cycles = 1;
    uint batch_process_freq = 1;
    uint min_ins = 1;
    uint max_ins = 1;
    uint delays_per_exec = 0;
    bool initialized = false;

    bool set(const str& key, str value) {
      if (key == "scheduler") {
        scheduler = move(value);
        return true;
      }

      if      (key == "num-cpu")            return (num_cpu = cast_uint(value)), true;
      else if (key == "quantum-cycles")     return (quantum_cycles = cast_uint(value)), true;
      else if (key == "batch-process-freq") return (batch_process_freq = cast_uint(value)), true;
      else if (key == "min-ins")            return (min_ins = cast_uint(value)), true;
      else if (key == "max-ins")            return (max_ins = cast_uint(value)), true;
      else if (key == "delays-per-exec")    return (delays_per_exec = cast_uint(value)), true;

      return false;
    }
  };
}
