#pragma once
#include "core/common/imports/_all.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"

namespace csopesy::command {
  inline const CommandHandler make_screen() {
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

      .execute = [](const Command &command, Shell &shell) {
        using Interpreter = InstructionInterpreter;
        using set = unordered_set<int>;

        auto& screen = shell.get_screen();
        if (!shell.get_screen().is_main())
          return void(cout << "Not in the Main Menu.\n");
        
        auto& interpreter = InstructionInterpreter::instance();
        auto& scheduler = shell.get_scheduler();
        auto& config = scheduler.get_config();

        if (command.flags.contains("-ls")) {
          const auto& running  = scheduler.get_running_processes();
          const auto& finished = scheduler.get_finished_processes();
          const auto& config = scheduler.get_config();
          
          set used_cores;
          for (const auto& proc_ref : running) {
            const auto& proc = proc_ref.get();
            if (proc.get_core() != -1)
              used_cores.insert(proc.get_core());
          }

          int total_cores = config.num_cpu;
          int used = used_cores.size();
          int free = total_cores - used;
          float cpu_util = (float)used / total_cores * 100;

          // Output
          cout << format("CPU Utilization: {:.2f}%\n", cpu_util);
          cout << format("Cores used: {} / {}\n\n", used, total_cores);

          cout << "\033[38;5;33m---------------------------------------------\033[0m\n";
          cout << "Running processes:\n";
          for (const auto& proc_ref : running) {
            const auto& proc = proc_ref.get();

            // ✅ Skip processes not currently assigned to a core
            if (proc.get_core() == -1) continue;

            cout << format(
              "  {:<10} \033[36m({})\033[0m  Core: {:<2}  \033[38;5;208m{} / {}\033[0m\n",
              proc.get_name(),
              timestamp(proc.get_start_time()),
              proc.get_core(),
              proc.get_program().get_ip(),
              proc.get_program().size()
            );
          }

          cout << "\nFinished processes:\n";
          for (const auto& proc_ref : finished) {
            const auto& proc = proc_ref.get();
            cout << format(
              "  {:<10} \033[36m({})\033[0m  Finished      \033[38;5;208m{} / {}\033[0m\n",
              proc.get_name(),
              timestamp(proc.get_start_time()),
              proc.get_program().size(),
              proc.get_program().size()
            );
          }

          cout << "\033[38;5;33m---------------------------------------------\033[0m\n";
        }

        else if (command.flags.contains("-s")) {
          const str& name = command.flags.at("-s");

          const auto& processes = scheduler.get_processes();
          // check if any process in the list has the same name
          bool process_exists = any_of(processes.begin(), processes.end(), [&](const auto& proc){
            return proc.get_name() == name;
          });

          if (process_exists)
            return void(cout << "Process " << name << " already exists\n");

          const auto& handlers = interpreter.get_handlers();
          const int pid = scheduler.get_processes().size() + 1;

          // create process 
          auto process = Process(name, pid);

          const int min = config.min_ins;
          const int max = config.max_ins;

          // process.set_program(randomized instruction) TODO
          const uint num_inst = Random::num(min, max);
          for (uint j = 0; j < num_inst; ++j) {
            auto handler = Random::pick(handlers).get();
            auto inst = handler.example(process.get_data());
            process.get_program().add_instruction(inst);
          }
          
          // scheduler.add_process(process);
          scheduler.add_process(move(process));

          // change screens and print info
          screen.switch_to(name);
          
          const auto& created = scheduler.get_processes().back();
          const auto& logs = created.get_logs();
          const auto& program = created.get_program();

          cout << "Process name: " << created.get_name() << '\n';
          cout << "ID: " << created.get_pid() << '\n';
          cout << "Logs:\n";
          for (const auto& log : logs) {
              cout << log << '\n';
          }
          cout << "Current instruction line: " << program.get_ip() << '\n';
          cout << "Lines of code: " << program.size() << '\n';
        }

        else if (command.flags.contains("-r")) {
          const str& name = command.flags.at("-r");
          const auto& running  = scheduler.get_running_processes();
          const auto& finished = scheduler.get_finished_processes();

          const Process* matched_proc = nullptr; 

          // check running processes
          for(const auto& proc_run : running) { 
            const auto& proc = proc_run.get();
            if (proc.get_name() == name) {
              matched_proc = &proc;
              break;
            }
          }
          
          // check finished processes
          if (!matched_proc) { 
            for (const auto& proc_ref : finished) {
              const auto& proc = proc_ref.get();
              if (proc.get_name() == name) {
                matched_proc = &proc;
                break;
              }
            }
          }

          if (!matched_proc)
            return void(cout << format("Process <{}> not found. \n", name));

            
          const auto& logs = matched_proc->get_logs();
          const auto& program = matched_proc->get_program();
          
          screen.switch_to(matched_proc->get_name());
          cout << "Process name: " << matched_proc->get_name() << '\n';
          cout << "ID: " << matched_proc->get_pid() << '\n';
          cout << "Logs:\n";

          for (const auto& log: logs)
            cout << log << '\n';
            
          cout << "Current instruction line: " << program.get_ip() << '\n';
          cout << "Lines of code: " << program.size() << '\n';
        } 
      },
    };
  }
}
