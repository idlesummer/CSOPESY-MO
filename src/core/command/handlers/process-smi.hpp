#pragma once
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"
#include "core/process/Process.hpp"
#include "core/execution/Core.hpp"

namespace csopesy::command {
  inline const CommandHandler make_process_smi() {
    using ProcessRef = Core::ProcessRef;

    return {
      .name = "process-smi",
      .desc = "Shows the current process status, logs, and info.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},
      
      .validate = [](const Command& command, Shell& shell) -> Str {
        if (shell.get_screen().is_main())
          return "Not in a process screen.";

        uint pid = shell.get_screen().get_id();
        const auto& scheduler = shell.get_scheduler();
        const auto& data = scheduler.get_data();

        if (!data.has_process(pid))
          return format("Active process with ID \"{}\" not found.", pid);

        auto process = data.get_process(pid); // Returns a process ref
        auto& storage = shell.get_storage();
        storage.set("process-smi.cache", process);
        return nullopt;
      },
      
      .execute = [](const Command& command, Shell& shell) {
        auto& storage = shell.get_storage();
        auto& ref = storage.get<ProcessRef>("process-smi.cache");

        const auto& process = ref.get();;
        const auto& logs = process.get_logs();
        const auto& program = process.get_program();

        cout << "Process name: " << process.get_name() << '\n';
        cout << "ID: " << process.get_id() << '\n';

        cout << "Logs:\n";
        for (const auto& log: logs)
          cout << log << '\n';

        // if (process.is_finished()) {
        //   cout << "Finished!\n\n";
        // 
        // } else {
        //   cout << "Current instruction line: " << program.get_ip() << '\n';
        //   cout << "Lines of code: " << program.size() << "\n\n";
        // }

        cout << "Current instruction line: " << program.get_ip() << '\n';
        cout << "Lines of code: " << program.size() << "\n\n";
        storage.remove("process-smi.cache");
      },
    };
  }
}
