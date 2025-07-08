#pragma once
#include "core/common/imports/_all.hpp"
#include "core/scheduler/SchedulerData.hpp"
#include "core/scheduler/SchedulerCore.hpp"
#include "core/process/Process.hpp"

namespace csopesy {
  
  /**
   * Represents a pluggable scheduling strategy (e.g., FCFS, Round Robin).
   * Delegates scheduling logic to functional components.
   */
  class SchedulerStrategy {
    
    public:
    using Job = SchedulerCore::Job;   ///< A process assigned to a core, or empty if unassigned.
    using AddHandler = function<void(Process&, SchedulerData&)>;
    using SelectHandler = function<Job(SchedulerData&)>;
    using TickHandler = function<void(SchedulerData&, const SelectHandler&)>;
    
    private: 
    AddHandler add;     ///< Called when a process is added.
    SelectHandler next; ///< Called to select the next process.
    TickHandler tick;   /// Called once per scheduler tick.

    public:
  
    // === Configuration ===

    /** Sets the handler for when a process is added. */
    SchedulerStrategy& on_add(AddHandler handler) {
      return add = move(handler), *this;
    }

    /** Sets the handler for selecting the next process. */
    SchedulerStrategy& on_select(SelectHandler handler) {
      return next = move(handler), *this;
    }

    /** Sets the handler for per-tick scheduler logic. */
    SchedulerStrategy& on_tick(TickHandler handler) {
      return tick = move(handler), *this;
    }

    // === Execution Interface ===

    /** Called by the scheduler when a new process is added. */
    void add(Process& process, SchedulerData& data) { add(process, data); }

    /** Called by the scheduler to choose the next process to schedule. */
    Job select(SchedulerData& data) { return next(data); }

    /** Called once per CPU tick to run any internal strategy logic. */
    void tick(SchedulerData& data) { tick(data, next); }    
  };
}
