#pragma once
#include "core/shell/Shell.hpp"
#include "core/command/types.hpp"

namespace csopesy::command {
  inline const CommandHandler make_report_util() {
    return {
      .name = "report-util",
      .desc = "Generates a CPU unitilization report.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},
       .execute = [](const Command &command, Shell &shell)
          {
              if (shell.get_screen() != Screen::MAIN_MENU) {
                  cout << "Not in the Main Menu.\n";
                  return;
              }

              auto& scheduler = shell.get_scheduler();
              const auto& running = scheduler.get_running_processes();
              const auto& finished = scheduler.get_finished_processes();

              ofstream log("csopesylog.txt");
              auto separator = "---------------------------------------------\n";

              cout << "\033[38;5;33m" << separator << "\033[0m";
              log << separator;

              cout << "Running processes:\n";
              log << "Running processes:\n";

              for (const auto& proc_ref : running) {
                  const auto& proc = proc_ref.get();
                  auto line = format(
                      "  {:<10} ({})  Core: {:<2}  {} / {}\n",
                      proc.get_name(),
                      timestamp(proc.get_start_time()),
                      proc.get_core(),
                      proc.get_program().get_ip(),
                      proc.get_program().size()
                  );
                  cout << "\033[36m" << line << "\033[0m";
                  log << line;
              }

              cout << "\nFinished processes:\n";
              log << "\nFinished processes:\n";

              for (const auto& proc_ref : finished) {
                  const auto& proc = proc_ref.get();
                  auto line = format(
                      "  {:<10} ({})  Finished      {} / {}\n",
                      proc.get_name(),
                      timestamp(proc.get_start_time()),
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

