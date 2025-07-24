#pragma once
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"
#include "core/process/Process.hpp"
#include "core/execution/Core.hpp"


auto make_process_smi() -> CommandHandler {
  return CommandHandler()
    .set_name("process-smi")
    .set_desc("Shows the current process status, logs, and info.")
    .set_min_args(0)
    .set_max_args(0)

    .set_validate([](Command& command, Shell& shell) -> Str {
      if (shell.screen.is_main())
        return "Not in a process screen.";

      auto& scheduler = shell.scheduler;
      uint pid = shell.screen.get_id();

      if (!scheduler.data.has_process(pid))
        return format("Active process with ID \"{}\" not found.", pid);

      return nullopt;
    })

    .set_execute([](Command& command, Shell& shell) {
      auto& scheduler = shell.scheduler;
      auto& storage = shell.storage;

      uint pid = shell.screen.get_id();
      auto& process = scheduler.data.get_process(pid);
      auto& program = process.data.program;

      cout << format("Process name: {}\n", process.data.name);
      cout << format("ID: {}\n", process.data.core_id);

      cout << "Logs:\n";
      for (auto& log: process.data.logs)
        cout << format("{}\n", log);

      cout << format("Current instruction line: {}\n", program.ip);
      cout << format("Lines of code: {}\n\n", program.script.size());

      shell.storage.remove("process-smi.pid");
    });
}
