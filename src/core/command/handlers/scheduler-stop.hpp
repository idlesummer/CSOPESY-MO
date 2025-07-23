#pragma once
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"


auto make_scheduler_stop() -> CommandHandler {
  return CommandHandler()
    .set_name("scheduler-stop")
    .set_desc("Stops generating dummy processes.")
    .set_min_args(0)
    .set_max_args(0)
    .set_flags({})
    
    .set_validate([](Command&, Shell& shell) -> Str {
      auto config = shell.scheduler.data.config;
      
      // Check if the scheduler has already been initialized
      if (!config.getb("initialized"))
        return "Scheduler not initialized. Please run 'initialize' first.";

      if (!shell.scheduler.generating)
        return "Dummy process generation is not running.";

      return nullopt;
    })
    
    .set_execute([](Command&, Shell& shell) {
      shell.scheduler.generate(false);
      cout << "[Shell] Dummy process generation stopped.\n";
    });
}
