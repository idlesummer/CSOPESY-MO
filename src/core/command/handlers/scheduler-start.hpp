#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"
#include "core/shell/Shell.hpp"

namespace csopesy::command {
  inline const CommandHandler make_scheduler_start() {
    return {
      .name = "scheduler-start",
      .desc = "Starts periodic dummy process generation every batch_process_freq ticks.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},

      .validate = [](const Command&, Shell& shell) -> Str {
        if (!shell.get_scheduler().is_initialized())
          return Str{"[Shell] Please run 'initialize' first."};

        if (shell.get_scheduler().is_generating())
          return Str{"[Shell] Dummy process generation already active."};

        return nullopt;
      },

      .execute = [](const Command&, Shell& shell) {
        shell.get_scheduler().start_generation();
        cout << "[Shell] Dummy process generation started.";
      },
    };
  }
}
