#pragma once
#include "core/common/imports/_all.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"


auto make_util() -> CommandHandler {
  return CommandHandler()
    .set_name("report-util")
    .set_desc("Generates a report of running and Finished processes to csopesylog.txt")
    .set_min_args(0)
    .set_max_args(0)
    .set_flags({})

    .set_validate([](Command&, Shell& shell) -> Str {
      if (!shell.screen.is_main())
        return "Not in the Main Menu.";
      return nullopt;
    })

    .set_execute([](Command&, Shell& shell) {
      auto& data = shell.scheduler.data;
      auto running = data.cores.get_running_pids();
      auto& finished = data.finished_pids;

      auto log = ofstream("csopesylog.txt");
      auto separator = "---------------------------------------------\n";

      cout << format("\033[38;5;33m{}\033[0m", separator);
      log << separator;

      cout << "Running processes:\n";
      log << "Running processes:\n";

      for (const auto& pid : running) {
        auto& process = data.get_process(pid);
        auto line = format(
          "  {:<10} ({})  Core: {:<2}  {} / {}\n",
          process.data.name,
          timestamp(process.data.stime),
          process.data.core_id,
          process.data.program.ip,
          process.data.program.size()
        );
        cout << format("\033[36m{}\033[0m", line);
        log << line;
      }

      cout << "\nFinished processes:\n";
      log << "\nFinished processes:\n";

      for (auto& pid: finished) {
        auto& process = data.get_process(pid);
        auto line = format(
          "  {:<10} ({})  Finished      {} / {}\n",
          process.data.name,
          timestamp(process.data.stime),
          process.data.program.size(),
          process.data.program.size()
        );
        cout << format("\033[36m{}\033[0m", line);
        log << line;
      }

      cout << format("\033[38;5;33m{}\033[0m", separator);
      log << separator;
      log.close();

      cout << "[report-util] Report written to csopesylog.txt\n";
    });
}
