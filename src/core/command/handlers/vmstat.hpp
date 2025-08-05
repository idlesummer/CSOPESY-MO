#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"


auto make_vmstat() -> CommandHandler {
  return CommandHandler()
    .set_name("vmstat")
    .set_desc("Provides a detailed view of the active/inactive processes, available/used memory, and pages.")
    .set_min_args(0)
    .set_max_args(0)
    
    .set_validate([](Command& command, Shell& shell) -> optional<str> {
      if (!shell.screen.is_main())
        return "Not in the Main Menu.";

      return nullopt;
    })

    .set_execute([](Command& command, Shell& shell) {
      cout << "To be implemented.\n";
    });
}
