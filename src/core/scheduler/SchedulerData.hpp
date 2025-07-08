#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"
#include "core/scheduler/SchedulerCore.hpp"

namespace csopesy {

  /**
   * Encapsulates scheduling context passed to strategy functions.
   * Provides access to the current core list, ready queue, and tick counter.
   */
  class SchedulerData {
    using list = vector<SchedulerCore>;
    using queue = queue<ref<Process>>;

    list& cores;    ///< Reference to the list of scheduler cores.
    queue& rqueue;  ///< Reference to the ready queue of processes.
    uint ticks;     ///< Current tick count.

    public:

    /** Constructs a SchedulerData context from external scheduler state. */
    SchedulerData(list& cores, queue& rqueue, uint ticks): 
      cores(cores), rqueue(rqueue), ticks(ticks) {}

    /** Returns a reference to the core list. */
    list& get_cores() { return cores; }
    const list& get_cores() const { return cores; }

    /** Returns a reference to the ready queue. */
    queue& get_rqueue() { return rqueue; }
    const queue& get_rqueue() const { return rqueue; }

    /** Returns the current tick count. */
    uint get_ticks() const { return ticks; }
  };
}
