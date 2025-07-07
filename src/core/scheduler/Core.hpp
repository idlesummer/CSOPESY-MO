#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"

namespace csopesy {
  class Core {
    using ProcessRef = ref<Process>;
    using Job = optional<ProcessRef>;

    Job job;
    uint id;

  public:

    Core(uint id=0): id(id) {}

    void assign(ProcessRef procref) {
      job = move(procref);
      auto& process = procref.get();
      process.set_core(id);
    }

    void release() {
      if (!job) return;

      auto& process = job->get();
      process.reset_core();
      job.reset();
    }

    void step() {
      if (!job) return;

      auto& process = job->get();
      if (process.step())
        release();
    }

    Process& get_job() {
      if (!job)
        throw runtime_error("No job assigned to core");
      return job->get();
    }

    uint id() const { return id; }
    bool is_idle() const { return !job.has_value(); }
    bool is_running() const { return job.has_value(); }
  };
}
