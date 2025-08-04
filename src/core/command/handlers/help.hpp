#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/command/handlers/_all.hpp"


auto make_help() -> CommandHandler {
  return CommandHandler()
    .set_name("help")
    .set_desc("Provides information of how the commands work")
    .set_min_args(0)
    .set_max_args(0)
    
    .set_validate([](Command& command, Shell& shell) -> optional<str> {
      if (!shell.screen.is_main())
        return "Not in the Main Menu.";

      auto config = shell.scheduler.data.config;
      if (!config.getb("initialized"))
        return "Scheduler not initialized. Please run 'initialize' first.";

      return nullopt;
    })

    .set_execute([](Command& command, Shell& shell) {
        cout << "To be implemented.\n";

    });
}
