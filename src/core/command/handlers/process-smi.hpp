#pragma once
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"
#include "core/process/Process.hpp"

namespace csopesy::command {
  inline const CommandHandler make_process_smi() {
    return {
      .name = "process-smi",
      .desc = "Shows the current process status, logs, and info.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},
      
      .validate = [](const Command& command, Shell& shell) -> Str {
        if (shell.get_screen().is_main())
          return "Not in a process screen.";

        const auto& name = shell.get_screen().get_id();
        const auto& scheduler = shell.get_scheduler();
        const auto& processes = scheduler.get_processes();

        auto it = find_if(processes, [&](const auto& proc) {
          return proc.get_name() == name;
        });

        if (it == processes.end())
          return format("Active process \"{}\" not found.", name);

        auto& storage = shell.get_storage();
        storage.set("process-smi.cache", ref(*it));
        return nullopt;
      },
      
      .execute = [](const Command& command, Shell& shell) {
        auto& storage = shell.get_storage();
        auto& target_ref = storage.get<ref<Process>>("process-smi.cache");

        const auto& process = target_ref.get();;
        const auto& logs = process.get_logs();
        const auto& program = process.get_program();

        cout << "Process name: " << process.get_name() << '\n';
        cout << "ID: " << process.get_pid() << '\n';

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
        cout << "Lines of code: " << program.size() << "\n" << "\n";
        storage.remove("process-smi.cache");
      },
    };
  }
}
