
#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"

namespace csopesy {
  class Core {
    using ProcessRef = ref<Process>;
    using Slot = optional<ProcessRef>;
    using func = function<void(Process&)>;

    Slot process;
    uint id;

    public:
    
    Core(uint id=0): id(id) {}

    void assign(ProcessRef p) {
      process = move(p);
      p.get().set_core(id);
    }
    
    void release() {
      if (process) 
        process.value().get().reset_core();
      process.reset();
    }
    
    void step() {
      if (!process) return;
      auto& p = process.value().get();

      if (p.step())
        release();
    }
    
    Process& get_process() {
      if (!process) 
        throw runtime_error("No process assigned to core");
      return process.value().get();
    }
    
    uint id() const { return id; }
    bool is_idle() const { return !process.has_value(); }
    bool is_running() const { return process.has_value(); }
  };
}
