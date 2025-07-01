#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/types.hpp"
#include "ProcessMemory.hpp"
#include "ProcessProgram.hpp"
#include "ProcessState.hpp"
#include "types.hpp"

namespace csopesy {

  /**
   * @brief Internal state container for a process.
   * Only accessible through the Process class.
   */
  class ProcessData {
    using list = vector<str>;

    str name;
    uint pid;
    int core = -1;
    Time start_time = Clock::now();
    ProcessState state;
    ProcessMemory memory;
    ProcessProgram program;
    list logs;

    friend class Process; // 👈 grants access to Process only
  };

} // namespace csopesy
