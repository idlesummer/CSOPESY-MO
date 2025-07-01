#pragma once
#include "core/shell/Shell.hpp"
#include "core/command/types.hpp"

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
        if (shell.get_screen() != Screen::PROCESS_VIEW) {
          cout << "Not in a process screen.\n";
          return;
        }

        const auto& name = *shell.get_active_process_name();
        const auto& processes = scheduler.get_processes();

        auto it = std::find_if(processes.begin(), processes.end(), [&](const auto& proc) {
          return proc.get_name() == name;
        });

        if (it == processes.end()) {
          std::cout << "Active process \"" << name << "\" not found.\n";
          return;
        }

        const auto& process = *it;
        const auto& logs = process.get_logs();
        const auto& program = process.get_program();

        std::cout << "Process name: " << process.get_name() << '\n';
        std::cout << "ID: " << process.get_pid() << '\n';

        std::cout << "Logs:\n";
        for (const auto& log : logs) {
          std::cout << log << '\n';
        }

        // if (process.is_finished()) {
        //   std::cout << "Finished!\n\n";
        // } else {
        //   std::cout << "Current instruction line: " << program.get_ip() << '\n';
        //   std::cout << "Lines of code: " << program.size() << "\n\n";
        // }

        std::cout << "Current instruction line: " << program.get_ip() << '\n';
        std::cout << "Lines of code: " << program.size() << "\n" << "\n";

      },
    };
  }
}
