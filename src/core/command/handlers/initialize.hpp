#pragma once
#include "core/common/imports/_all.hpp"
#include "core/common/constants/banner.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"
#include "core/scheduler/types.hpp"

namespace csopesy::command {
  inline const CommandHandler make_initialize() {
    using list = vector<str>;
    return {
      .name = "initialize",
      .desc = "Initializes the processor configuration of the application.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},

      .validate = [](const Command& command, Shell& shell) -> Str {
        // Check if the scheduler has already been initialized
        if (shell.get_scheduler().get_config().initialized)
          return Str{"[Shell] Already initialized."};
        
        // Check if the file could not be opened or is empty
        auto lines = read_lines("config.txt");
        if (lines.empty())
          return "[Shell] Failed to open config.txt";

        command.context = any(move(lines));
        return nullopt;
      },

      .execute = [](const Command& command, Shell& shell) {
        const auto& lines = command.get_context<list>();
        auto config = SchedulerConfig();

        for (const auto& line: lines) {
          str key, value;
          isstream(line) >> key >> value;

          if (!config.set(key, move(value)))
            cout << format("[Shell] Unknown config key: {}\n", key);
        }

        config.initialized = true;
        shell.get_scheduler().set_config(config);  // ✅ Apply to scheduler

        cout << BANNER << endl;
        cout << "[Shell] Scheduler config loaded.";
      },
    };
  }
}
