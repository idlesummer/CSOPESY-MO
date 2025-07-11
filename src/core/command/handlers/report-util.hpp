#pragma once
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"

namespace csopesy::command {
  inline const CommandHandler make_report_util() {
    return {
      .name = "report-util",
      .desc = "Generates a CPU unitilization report.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},

      .validate = [](const Command &command, Shell &shell) -> Str {
        if (!shell.get_screen().is_main())
          return "Not in the Main Menu.";
        
        // Check if the scheduler is initialized
        if (!shell.get_scheduler().is_initialized())
          return "Scheduler not initialized. Please run 'initialize' first.";

        return nullopt; // No validation errors
      },

      .execute = [](const Command &command, Shell &shell) {
        const auto& data = shell.get_scheduler().get_data();
        const auto& running = data.get_running_processes();
        const auto& finished = data.get_finished_processes();

        auto log = ofstream("csopesylog.txt");
        auto separator = "---------------------------------------------\n";

        cout << "\033[38;5;33m" << separator << "\033[0m";
        log << separator;

        cout << "Running processes:\n";
        log << "Running processes:\n";

        for (const auto& ref: running) {
          const auto& proc = ref.get();
          auto line = format(
            "  {:<10} ({})  Core: {:<2}  {} / {}\n",
            proc.get_name(),
            timestamp(proc.get_stime()),
            proc.get_core(),
            proc.get_program().get_ip(),
            proc.get_program().size()
          );
          cout << "\033[36m" << line << "\033[0m";
          log << line;
        }

        cout << "\nFinished processes:\n";
        log << "\nFinished processes:\n";

        for (const auto& ref: finished) {
          const auto& proc = ref.get();
          auto line = format(
            "  {:<10} ({})  Finished      {} / {}\n",
            proc.get_name(),
            timestamp(proc.get_stime()),
            proc.get_program().size(),
            proc.get_program().size()
          );
          cout << "\033[36m" << line << "\033[0m";
          log << line;
        }

        cout << "\033[38;5;33m" << separator << "\033[0m";
        log << separator;
        log.close();

        cout << "[report-util] Report written to csopesylog.txt\n";
      }
    };
  }
}
