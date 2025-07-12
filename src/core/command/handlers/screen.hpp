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

        if (!shell.get_screen().is_main())
          return "Not in the Main Menu.";

        return nullopt;
      },

      .execute = [&](const Command& command, Shell& shell) {
        using set = unordered_set<int>;

        auto& screen = shell.get_screen();
        auto& scheduler = shell.get_scheduler();

        // === -ls: List screen info
        if (command.flags.contains("-ls")) {
          access([&] {
            auto& data = scheduler.get_data();
            const auto& config = scheduler.get_config();

            const auto& busy_cores = data.get_cores().get_busy();
            unordered_set<uint> running;

            // Collect running PIDs and core usage
            set used_cores;
            for (const auto& ref : busy_cores) {
              auto& core = ref.get();

              if (!core.is_idle()) {
                uint pid = core.get_job().get_id();
                running.insert(pid);
                used_cores.insert(core.get_id());
              }
            }

            int total_cores = config.num_cpu;
            int used = used_cores.size();
            float cpu_util = total_cores ? (float)used / total_cores * 100.0f : 0.0f;

            cout << format("CPU Utilization: {:.2f}%\n", cpu_util);
            cout << format("Cores used: {} / {}\n\n", used, total_cores);
            cout << "\033[38;5;33m---------------------------------------------\033[0m\n";
            cout << "Running processes:\n";

            for (uint pid : running) {
              const auto& proc = data.get_process(pid).get();

              cout << format(
                "  {:<10} \033[36m({})\033[0m  Core: {:<2}  \033[38;5;208m{} / {}\033[0m\n",
                proc.get_name(),
                timestamp(proc.get_stime()),
                proc.get_core(),
                proc.get_program().get_ip(),
                proc.get_program().size()
              );
            }

            cout << "\nFinished processes:\n";
            for (uint pid : data.get_finished()) {
              const auto& proc = data.get_process(pid).get();
              cout << format(
                "  {:<10} \033[36m({})\033[0m  Finished      \033[38;5;208m{} / {}\033[0m\n",
                proc.get_name(),
                timestamp(proc.get_stime()),
                proc.get_program().size(),
                proc.get_program().size()
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
              const auto& data = scheduler.get_data();
              return data.has_process(name);
            });
          };

          if (process_exists(name))
            return void(cout << format("Process '{}' already exists\n", name));

          access([&] {
            scheduler.enqueue_process(name);
          });

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
            auto& data = scheduler.get_data();
            const auto& proc = data.get_process(name).get();
            uint pid = proc.get_id(); // ✅ Extract PID

            screen.switch_to(pid);    // ✅ Use only PID
            cout << "\n";
            cout << format("Process name: {}\n", proc.get_name());
            cout << format("ID: {}\n", pid);

            cout << "Logs:\n";
            for (const auto& log : proc.get_logs())
              cout << format("  {}\n", log);

            const auto& program = proc.get_program();
            cout << format("Current instruction line: {}\n", program.get_ip());
            cout << format("Lines of code: {}\n", program.size());
          });
        }

        // === -r: Resume process by name
        else if (command.flags.contains("-r")) {
          access([&] {
            const auto& name = command.flags.at("-r");
            auto& data = scheduler.get_data();

            if (!data.has_process(name))
              return void(cout << format("Process <{}> not found.\n", name));

            const auto& proc = data.get_process(name).get();
            uint pid = proc.get_id(); // ✅ Use PID

            screen.switch_to(pid);    // ✅ PID-based only

            cout << format("Process name: {}\n", proc.get_name());
            cout << format("ID: {}\n", pid);

            cout << "Logs:\n";
            for (const auto& log : proc.get_logs())
              cout << format("  {}\n", log);

            cout << format("Current instruction line: {}\n", proc.get_program().get_ip());
            cout << format("Lines of code: {}\n", proc.get_program().size());
          });
        }
      }
    };
  }
}
