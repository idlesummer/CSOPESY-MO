#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"


auto make_report_util() -> CommandHandler {
  return CommandHandler()
    .set_name("report-util")
    .set_desc("Generates a CPU unitilization report.")
    .set_min_args(0)
    .set_max_args(0)
    
    .set_validate([](Command& command, Shell& shell) -> Str {
      if (!shell.screen.is_main())
        return "Not in the Main Menu.";

      auto config = shell.scheduler.data.config;
      if (!config.getb("initialized"))
        return "Scheduler not initialized. Please run 'initialize' first.";

      return nullopt;
    })

    .set_execute([](Command& command, Shell& shell) {
      auto& scheduler = shell.scheduler;
      auto& data = scheduler.data;
      auto running = data.get_running_pids();
      auto& finished = data.finished_pids;

      auto log = ofstream("csopesylog.txt");
      auto separator = "──────────────────────────────────────────────────────────\n";

      cout << format("\033[38;5;33m{}\033[0m", separator);
      log << separator;

      cout << "Running processes:\n";
      log << "Running processes:\n";

      for (auto pid: running) {
        auto& proc = data.get_process(pid);
        auto line = format(
          "  {:<10} ({})  Core: {:<2}  {} / {}\n",
          proc.data.name,
          timestamp(proc.data.stime),
          proc.data.core_id,
          proc.data.program.ip,
          proc.data.program.size()
        );
        cout << "\033[36m" << line << "\033[0m";
        log << line;
      }

      cout << "\nFinished processes:\n";
      log << "\nFinished processes:\n";

      for (auto pid: finished) {
        auto& proc = data.get_process(pid);
        auto line = format(
          "  {:<10} ({})  Finished      {} / {}\n",
          proc.data.name,
          timestamp(proc.data.stime),
          proc.data.program.size(),
          proc.data.program.size()
        );
        cout << "\033[36m" << line << "\033[0m";
        log << line;
      }

      cout << "\033[38;5;33m" << separator << "\033[0m";
      log << separator;
      log.close();

      cout << "[report-util] Report written to csopesylog.txt\n";
    });
}
