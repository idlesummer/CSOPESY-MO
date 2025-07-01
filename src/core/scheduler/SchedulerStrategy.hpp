#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"

namespace csopesy {
  class SchedulerStrategy {
    using Selection = Ref<Process>;
    using Selector  = function<Selection()>;
    using Inserter  = function<void(Process&)>;
    using Ticker    = function<void()>;

    Inserter on_add;
    Selector next_process;
    Ticker   on_tick;

    public:
    SchedulerStrategy(Inserter add, Selector next, Ticker tick = []{}):
      on_add(move(add)), 
      next_process(move(next)), 
      on_tick(move(tick)) {}

    void add(Process& p)    { on_add(p); }
    Selection select_next() { return next_process(); }
    void tick()             { on_tick(); }
  };
}
