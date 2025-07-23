#pragma once
#include "core/common/imports/_all.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/process/Process.hpp"
#include "core/shell/internal/Shell.impl.hpp"


auto make_scheduler_start() -> CommandHandler {
  return CommandHandler()
    .set_name("scheduler-start")
    .set_desc("Starts periodic dummy process generation every 'batch_process_freq' ticks.")
    .set_min_args(0)
    .set_max_args(0)
    .set_flags({})
    
    .set_validate([](Command&, Shell& shell) -> Str {
      auto config = shell.scheduler.data.config;
      
      // Check if the scheduler has already been initialized
      if (!config.getb("initialized"))
        return "Scheduler not initialized. Please run 'initialize' first.";

      if (shell.scheduler.generating)
        return "Dummy process generation already active.";

      return nullopt;
    })
    
    .set_execute([](Command&, Shell& shell) {
      shell.scheduler.generate(true);
      cout << "[Shell] Dummy process generation started.\n";
    });
}
