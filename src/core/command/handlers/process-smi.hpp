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
          if (shell.get_screen().is_main())
            return "Not in a process screen.";

          uint pid = shell.get_screen().get_id();
          const auto& scheduler = shell.get_scheduler();
          const auto& data = scheduler.get_data();

          if (!data.has_process(pid))
            return format("Active process with ID \"{}\" not found.", pid);

          shell.get_storage().set("process-smi.pid", pid);  // ✅ Store PID instead of ref
          return nullopt;
        });
      },

      .execute = [](const Command& command, Shell& shell) {
        access([&] {
          const auto& scheduler = shell.get_scheduler();
          const auto& data = scheduler.get_data();
          auto& storage = shell.get_storage();

          uint pid = storage.get<uint>("process-smi.pid"); // ✅ Retrieve PID
          const auto& proc = data.get_process(pid).get();  // ✅ Access by PID

          const auto& logs = proc.get_logs();
          const auto& program = proc.get_program();

          cout << "Process name: " << proc.get_name() << '\n';
          cout << "ID: " << proc.get_id() << '\n';

          cout << "Logs:\n";
          for (const auto& log: logs)
            cout << log << '\n';

          cout << "Current instruction line: " << program.get_ip() << '\n';
          cout << "Lines of code: " << program.size() << "\n\n";

          shell.get_storage().remove("process-smi.pid");
        });
      },
    };
  }
}
