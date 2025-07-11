#pragma once
#include "core/common/imports/_all.hpp"
#include "core/execution/Core.hpp"
#include "core/scheduler/SchedulerData.hpp"
#include "core/process/Process.hpp"

namespace csopesy {
  
  /**
   * Represents a pluggable scheduling strategy (e.g., FCFS, Round Robin).
   * Delegates scheduling logic to functional components.
   */
  class SchedulerStrategy {
    
    public:
    using Job = Core::Job;          ///< A process assigned to a core, or empty if unassigned
    using AddHandler = function<void(Process&, SchedulerData&)>;
    using SelectHandler = function<Job(SchedulerData&)>;
    using PreemptHandler = function<void(SchedulerData&)>;
    using TickHandler = function<void(SchedulerData&, const SelectHandler&)>;
    
    private: 
    str name;                       ///< Name of the scheduling algorithm
    AddHandler handle_add;          ///< Called when a process is added
    SelectHandler handle_next;      ///< Called to select the next process to run 
    PreemptHandler handle_preempt;  ///< Called to determine which processes should be preempted
    TickHandler handle_tick;        ///< Called to assign ready jobs to idle cores
    
    public:
  
    /** Sets the name of the scheduler algorithm. */
    SchedulerStrategy(str name): name(name) {}
    SchedulerStrategy() = delete;

    /** Returns the name of the scheduler algorithm. */
    const str& get_name() const { return name; }

    // === Configuration ===

    /** Sets the handler for when a process is added. */
    SchedulerStrategy& on_add(AddHandler handler) {
      return handle_add = move(handler), *this;
    }

    /** Sets the handler for selecting the next process. */
    SchedulerStrategy& on_select(SelectHandler handler) {
      return handle_next = move(handler), *this;
    }

    /** Sets the handler for preemption logic. */
    SchedulerStrategy& on_preempt(PreemptHandler handler) {
      return handle_preempt = move(handler), *this;
    }

    /** Sets the handler to be called each scheduler::tick. */
    SchedulerStrategy& on_tick(TickHandler handler) {
      return handle_tick = move(handler), *this;
    }

    // === Execution Interface ===

    /** Called by the scheduler when a new process is added. */
    void add(Process& process, SchedulerData& data) {
      if (!handle_add) throw runtime_error("SchedulerStrategy::add called without an on_add handler.");
      handle_add(process, data);
    }

    /** Called by the scheduler to evaluate and trigger preemptions. */
    void preempt(SchedulerData& data) {
      if (!handle_preempt) return;  // Optional handler, no error if missing
      handle_preempt(data);
    }

    /** Called by the scheduler to choose the next process to schedule. */
    Job select(SchedulerData& data) {
      if (!handle_next) throw runtime_error("SchedulerStrategy::select called without an on_select handler.");
      return handle_next(data);
    }

    /** Executes the scheduling logic for the current tick. */
    void tick(SchedulerData& data) {
      if (!handle_tick) throw runtime_error("SchedulerStrategy::assign called without an on_assign handler.");
      handle_tick(data, handle_next);
    }
  };
}
