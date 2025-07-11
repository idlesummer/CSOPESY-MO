#pragma once
#include "core/common/imports/_all.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/scheduler/SchedulerData.hpp"
#include "core/shell/internal/Shell.impl.hpp"

namespace csopesy::command {
  inline const CommandHandler make_screen() {

    auto wait_process = [](const str& name, const SchedulerData& data) -> bool {     
      auto start = Clock::now();
      while (!data.has_process(name)) {
        if (Clock::now() - start > 3s)
          return false;
        cout << '.' << flush;
        sleep_for(100ms);
      }
      cout << "\n";
      return true;
    };

    return {
      .name = "screen",
      .desc = "Creates and switches through existing screens.",
      .min_args = 0,
      .max_args = 0,
      .flags = {{"-s", true}, {"-r", true}, {"-ls", false}},

      .validate = [](const Command &command, Shell &) -> Str {
        const bool has_ls = command.flags.contains("-ls");
        const bool has_s = command.flags.contains("-s");
        const bool has_r = command.flags.contains("-r");

        // If more than one of -s, -r, or -ls is used, return an error
        if (has_s + has_ls + has_r > 1)
          return "You must use only one of -s, -r, or -ls.";
        return nullopt;
      },

      .execute = [&](const Command &command, Shell &shell) {
        using Interpreter = InstructionInterpreter;
        using set = unordered_set<int>;

        auto& screen = shell.get_screen();
        if (!shell.get_screen().is_main())
          return void(cout << "Not in the Main Menu.\n");
        
        auto& interpreter = InstructionInterpreter::instance();
        auto& scheduler = shell.get_scheduler();
        auto& data = scheduler.get_data();
        auto& config = scheduler.get_config();

        if (command.flags.contains("-ls")) {
          const auto& running  = data.get_running_processes();
          const auto& finished = data.get_finished_processes();
          
          set used_cores;
          for (const auto& ref: running) {
            const auto& proc = ref.get();
            if (proc.get_core() != -1)
              used_cores.insert(proc.get_core());
          }

          int total_cores = config.num_cpu;
          int used = used_cores.size();
          int free = total_cores - used;
          float cpu_util = total_cores ? (float)used / total_cores * 100 : 0.0f;  // Divide by zero guard

          // Output
          cout << format("CPU Utilization: {:.2f}%\n", cpu_util);
          cout << format("Cores used: {} / {}\n\n", used, total_cores);

          cout << "\033[38;5;33m---------------------------------------------\033[0m\n";
          cout << "Running processes:\n";
          for (const auto& ref: running) {
            const auto& proc = ref.get();

            // ✅ Skip processes not currently assigned to a core
            if (proc.get_core() == -1) continue;

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
          for (const auto& ref: finished) {
            const auto& proc = ref.get();
            cout << format(
              "  {:<10} \033[36m({})\033[0m  Finished      \033[38;5;208m{} / {}\033[0m\n",
              proc.get_name(),
              timestamp(proc.get_stime()),
              proc.get_program().size(),
              proc.get_program().size()
            );
          }

          cout << "\033[38;5;33m---------------------------------------------\033[0m\n";
        }

        else if (command.flags.contains("-s")) {
          const str& name = command.flags.at("-s");
          
          // Check if any process in the list has the same name
          if (data.has_process(name))
            return void(cout << format("Process {} already exists\n", name));
          
          // Enqueue process for generation in the next scheduler tick
          scheduler.enqueue_process(name);
          
          // Wait for process to be created (blocking loop)
          cout << format("\nWaiting for process creation: ", name) << flush;
          if (wait_process(name, data)) 
            return void(cout << "\nTimed out.\n");
          
          // Retrieve the process by name
          const auto& proc = data.get_process(name).get(); 
          
          // Change screens and print info
          screen.switch_to(proc.get_id());
          
          // Output process info
          cout << format("Process name: {}\n", proc.get_name());
          cout << format("ID: {}\n", proc.get_id());
          
          cout << "Logs:\n";
          for (const auto& log : proc.get_logs())
            cout << format("  {}\n", log);
          
          const auto& program = proc.get_program();
          cout << format("Current instruction line: {}\n", program.get_ip());
          cout << format("Lines of code: {}\n", program.size());
        }

        else if (command.flags.contains("-r")) {
          const str& name = command.flags.at("-r");

          // Attempt to get a reference to the process by name
          if (!data.has_process(name))
            return void(cout << format("Process <{}> not found.\n", name));
          
          const auto& proc = data.get_process(name).get();
          const auto& logs = proc.get_logs();
          const auto& program = proc.get_program();

          // Switch screen and show process info
          screen.switch_to(proc.get_id());

          cout << format("Process name: {}\n", proc.get_name());
          cout << format("ID: {}\n", proc.get_id());

          cout << "Logs:\n";
          for (const auto& log: logs)
            cout << format("  {}\n", log);

          cout << format("Current instruction line: {}\n", program.get_ip());
          cout << format("Lines of code: {}\n", program.size());
            } 
      },
    };
  }
}
