#pragma once
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"

namespace csopesy::command {
  inline const CommandHandler make_process_smi() {
    return {
      .name = "process-smi",
      .desc = "Shows the current process status, logs, and info.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},
      
      .execute = [](const Command& command, Shell& shell) {
        auto& scheduler = shell.get_scheduler();
        
        if (shell.get_screen() != Screen::PROCESS)
          return void(cout << "Not in a process screen.\n");

        const auto& name = *shell.get_active_process_name();
        const auto& processes = scheduler.get_processes();

        auto it = find_if(processes, [&](const auto& proc) {
          return proc.get_name() == name;
        });

        if (it == processes.end())
          return void(cout << "Active process \"" << name << "\" not found.\n");

        const auto& process = *it;
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
      },
    };
  }
}
