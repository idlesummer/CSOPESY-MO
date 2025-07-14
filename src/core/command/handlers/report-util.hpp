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

      .validate = [](const Command& command, Shell& shell) -> Str {
        if (!shell.screen.is_main())
          return "Not in the Main Menu.";
        
        return access([&]() -> Str {
          if (!shell.scheduler.data.config.initialized)
            return "Scheduler not initialized. Please run 'initialize' first.";
          return nullopt;
        });
      },

      .execute = [](const Command& command, Shell& shell) {
        access([&] {
          auto& scheduler = shell.scheduler;
          auto& data = scheduler.data;
          auto running = data.get_running_pids();
          auto& finished = data.finished_pids;

          auto log = ofstream("csopesylog.txt");
          auto separator = "---------------------------------------------\n";

          cout << "\033[38;5;33m" << separator << "\033[0m";
          log << separator;

          cout << "Running proc_table:\n";
          log << "Running proc_table:\n";

          for (uint pid: running) {
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

          cout << "\nFinished proc_table:\n";
          log << "\nFinished proc_table:\n";

          for (uint pid: finished) {
            const auto& proc = data.get_process(pid);
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
    };
  }
}
