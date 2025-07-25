#pragma once
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"

namespace csopesy::command {
  inline const CommandHandler make_scheduler_stop() {
    return {
      .name = "scheduler-stop",
      .desc = "Stops generating dummy processes.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},

      .validate = [](const Command&, Shell& shell) -> Str {
        if (!shell.get_scheduler().is_initialized())
          return "[Shell] Please run 'initialize' first.";

        if (!shell.get_scheduler().is_generating())
          return "[Shell] Dummy process generation is not running.";
        
        return nullopt;
      },

      .execute = [](const Command&, Shell& shell) {
        shell.get_scheduler().generate(false);
        cout << "[Shell] Dummy process generation stopped.\n";
      },
    };
  }
}
