#pragma once
#include "core/common/imports/_all.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/scheduler/SchedulerData.hpp"
#include "core/shell/internal/Shell.impl.hpp"

namespace csopesy::command {
  inline const CommandHandler make_screen() {
    return {
      .name = "screen",
      .desc = "Creates and switches through existing screens.",
      .min_args = 0,
      .max_args = 0,
      .flags = {{"-s", true}, {"-r", true}, {"-ls", false}},

      .validate = [](const Command& command, Shell& shell) -> Str {
        const bool has_ls = command.flags.contains("-ls");
        const bool has_s  = command.flags.contains("-s");
        const bool has_r  = command.flags.contains("-r");

        if (has_s + has_ls + has_r > 1)
          return "You must use only one of -s, -r, or -ls.";

        if (!shell.screen.is_main())
          return "Not in the Main Menu.";

        return nullopt;
      },

      .execute = [&](const Command& command, Shell& shell) {
        using set = unordered_set<uint>;

        auto& screen = shell.screen;
        auto& scheduler = shell.scheduler;

        // === -ls: List screen info
        if (command.flags.contains("-ls")) {
          access([&] {
            auto& data = scheduler.data;
            auto& config = data.config;
            auto& cores = data.cores;

            uint size = cores.size();
            uint busy = cores.get_busy().size();
            float cpu_util = cores.get_usage();

            cout << format("CPU Utilization: {:.2f}%\n", cpu_util);
            cout << format("Cores used: {} / {}\n\n", busy, size);
            cout << "\033[38;5;33m---------------------------------------------\033[0m\n";
            cout << "Running processes:\n";

            for (uint pid: cores.get_running_pids()) {
              auto& process = data.get_process(pid);

              cout << format(
                "  {:<10} \033[36m({})\033[0m  Core: {:<2}  \033[38;5;208m{} / {}\033[0m\n",
                process.data.name,
                timestamp(process.data.stime),
                process.data.core_id,
                process.data.program.ip,
                process.data.program.size()
              );
            }

            cout << "\nFinished processes:\n";

            for (uint pid: data.finished_pids) {
              auto& process = data.get_process(pid);
              cout << format(
                "  {:<10} \033[36m({})\033[0m  Finished      \033[38;5;208m{} / {}\033[0m\n",
                process.data.name,
                timestamp(process.data.stime),
                process.data.program.size(),
                process.data.program.size()
              );
            }

            cout << "\033[38;5;33m---------------------------------------------\033[0m\n";
          });
        }

        // === -s: Spawn and switch to new process screen
        else if (command.flags.contains("-s")) {
          const str& name = command.flags.at("-s");

          auto process_exists = [&](const str& name) -> bool {
            return access([&] {
              const auto& data = scheduler.data;
              return data.has_process(name);
            });
          };

          if (process_exists(name))
            return void(cout << format("Process '{}' already exists\n", name));

          access([&] { scheduler.enqueue_process(name); });

          cout << format("\nWaiting for process creation: {}", name);
          bool created = false;

          for (uint i = 0; i < 30; ++i) {
            if (process_exists(name)) {
              created = true;
              break;
            }
            sleep_for(200ms);
          }

          if (!created)
            return void(cout << "\nTimed out.\n");

          access([&] {
            auto& data = scheduler.data;
            auto& process = data.get_process(name);
            uint pid = process.data.id; // ✅ Extract PID

            screen.switch_to(pid);    // ✅ Use only PID
            cout << "\n";
            cout << format("Process name: {}\n", process.data.name);
            cout << format("ID: {}\n", pid);

            cout << "Logs:\n";
            for (auto& log : process.data.logs)
              cout << format("  {}\n", log);

            const auto& program = process.data.program;
            cout << format("Current instruction line: {}\n", program.ip);
            cout << format("Lines of code: {}\n", program.size());
          });
        }

        // === -r: Resume process by name
        else if (command.flags.contains("-r")) {
          access([&] {
            const auto& name = command.flags.at("-r");
            auto& data = scheduler.data;

            if (!data.has_process(name))
              return void(cout << format("Process <{}> not found.\n", name));

            auto& process = data.get_process(name);
            uint pid = process.data.id; // ✅ Use PID

            screen.switch_to(pid);      // ✅ PID-based only
            cout << format("Process name: {}\n", process.data.name);
            cout << format("ID: {}\n", pid);

            cout << "Logs:\n";
            for (const auto& log : process.data.logs)
              cout << format("  {}\n", log);

            cout << format("Current instruction line: {}\n", process.data.program.ip);
            cout << format("Lines of code: {}\n", process.data.program.size());
          });
        }
      }
    };
  }
}
