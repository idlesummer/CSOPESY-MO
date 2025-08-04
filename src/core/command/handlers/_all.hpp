#pragma once
#include "core/common/imports/_all.hpp"
#include "core/command/CommandHandler.hpp"
#include "cls.hpp"
#include "config.hpp"
#include "demo.hpp"
#include "echo.hpp"
#include "exit.hpp"
#include "initialize.hpp"
#include "process-smi.hpp"
#include "report-util.hpp"
#include "scheduler-start.hpp"
#include "scheduler-stop.hpp"
#include "screen.hpp"
#include "vmstat.hpp"


auto get_command_handlers() -> vec<CommandHandler> {
  return {
    make_cls(),
    make_config(),
    make_demo(),
    make_echo(),
    make_exit(),
    make_initialize(),
    make_process_smi(),
    make_report_util(), 
    make_scheduler_start(),
    make_scheduler_stop(),
    make_screen(),
    make_vmstat(),
  };
}
