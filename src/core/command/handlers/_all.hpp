#pragma once
#include "core/command/types.hpp"
#include "clear.hpp"
#include "echo.hpp"
#include "exit.hpp"
#include "initialize.hpp"
#include "process-smi.hpp"
#include "report-util.hpp"
#include "scheduler-start.hpp"
#include "scheduler-stop.hpp"
#include "screen.hpp"

namespace csopesy::command {
  using list = vector<CommandHandler>;
  
  list get_all() {
    return {
      make_clear(),
      make_echo(),
      make_exit(),
      make_initialize(),
      make_report_util(),
      make_scheduler_start(),
      make_scheduler_stop(),
      make_screen(),
      make_process_smi(),
    };
  }
}
