#pragma once
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"
#include "core/process/Process.hpp"
#include "core/execution/Core.hpp"

namespace csopesy::command {
  inline const CommandHandler make_process_smi() {
    return {
      .name = "process-smi",
      .desc = "Shows the current process status, logs, and info.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},

      .validate = [](const Command& command, Shell& shell) -> Str {
        return access([&]() -> Str {
          if (shell.screen.is_main())
            return "Not in a process screen.";

            auto& scheduler = shell.scheduler;
            uint pid = shell.screen.get_id();

          if (!scheduler.data.has_process(pid))
            return format("Active process with ID \"{}\" not found.", pid);
          return nullopt;
        });
      },

      .execute = [](const Command& command, Shell& shell) {
        access([&] {
          auto& scheduler = shell.scheduler;
          auto& storage = shell.storage;

          uint pid = shell.screen.get_id();                 // ✅ Retrieve PID
          auto& process = scheduler.data.get_process(pid);  // ✅ Access by PID
          auto& program = process.data.program;

          cout << format("Process name: {}\n", process.data.name);
          cout << format("ID: {}\n", process.data.core_id);

          cout << "Logs:\n";
          for (auto& log: process.data.logs)
            cout << log << '\n';

          cout << "Current instruction line: " << program.ip << '\n';
          cout << "Lines of code: " << program.script.size() << "\n\n";

          shell.storage.remove("process-smi.pid");
        });
      },
    };
  }
}
