#pragma once
#include "core/common/imports/_all.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/process/Process.hpp"
#include "core/shell/internal/Shell.impl.hpp"

namespace csopesy::command {
  inline const CommandHandler make_scheduler_start() {
    return {
      .name = "scheduler-start",
      .desc = "Starts periodic dummy process generation every 'batch_process_freq' ticks.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},

      .validate = [](Command&, Shell& shell) -> Str {
        return access([&]() -> Str {
          if (!shell.scheduler.data.config.initialized)
            return "[Shell] Please run 'initialize' first.";
  
          if (shell.scheduler.generating)
            return "[Shell] Dummy process generation already active.";
  
          return nullopt;
        });
      },

      .execute = [](Command&, Shell& shell) {
        access([&] {
          shell.scheduler.generate(true);
          cout << "[Shell] Dummy process generation started.\n";
        });
      },
    };
  }
}
