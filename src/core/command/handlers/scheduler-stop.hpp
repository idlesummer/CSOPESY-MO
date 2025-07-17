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
      if (!shell.scheduler.data.config.initialized)
        return "[Shell] Please run 'initialize' first.";

      if (!shell.scheduler.generating)
        return "[Shell] Dummy process generation is not running.";

      return nullopt;
    })
    
    .set_execute([](Command&, Shell& shell) {
      shell.scheduler.generate(false);
      cout << "[Shell] Dummy process generation stopped.\n";
    });
}
