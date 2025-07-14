#pragma once
#include "core/common/imports/_all.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"

namespace csopesy::command {
  inline const CommandHandler make_util() {
    return {
      .name = "report-util",
      .desc = "Generates a report of running and finished proc_table to csopesylog.txt",
      .min_args = 0,
      .max_args = 0,
      .flags = {},

      .execute = [](const Command &command, Shell &shell) {   
        if (!shell.screen.is_main())
          return void(cout << "Not in the Main Menu.\n");

        auto& data = shell.scheduler.data;
        auto running = data.cores.get_running_pids();
        auto& finished = data.finished_pids;

        auto log = ofstream("csopesylog.txt");
        auto separator = "---------------------------------------------\n";

        cout << "\033[38;5;33m" << separator << "\033[0m";
        log << separator;

        cout << "Running proc_table:\n";
        log << "Running proc_table:\n";

        for (const auto& pid: running) {
          auto& process = data.get_process(pid);
          auto line = format(
            "  {:<10} ({})  Core: {:<2}  {} / {}\n",
            process.data.name,
            timestamp(process.data.stime),
            process.data.core_id,
            process.data.program.ip,
            process.data.program.size()
          );
          cout << "\033[36m" << line << "\033[0m";
          log << line;
        }

        cout << "\nFinished proc_table:\n";
        log << "\nFinished proc_table:\n";

        for (const auto& pid: finished) {
          auto& process = data.get_process(pid);
          auto line = format(
            "  {:<10} ({})  Finished      {} / {}\n",
            process.data.name,
            timestamp(process.data.stime),
            process.data.program.size(),
            process.data.program.size()
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