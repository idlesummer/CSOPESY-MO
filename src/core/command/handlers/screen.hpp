#pragma once
#include "core/common/imports/_all.hpp"
#include "core/common/utility/Table.hpp"
#include "core/common/utility/Text.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/scheduler/SchedulerData.hpp"
#include "core/shell/internal/Shell.impl.hpp"

/**
──────────────────────────────────────────────────────────
Running processes:
  p04        (07/26/2025 03:49:47AM)  Core: 1   7 / 11
  p09        (07/26/2025 03:49:48AM)  Core: 2   6 / 10
  p05        (07/26/2025 03:49:47AM)  Core: 3   5 / 11
  p02        (07/26/2025 03:49:47AM)  Core: 4   4 / 7

Finished processes:
  p01        (07/26/2025 03:49:47AM)   Finished      1 / 1
  p06        (07/26/2025 03:49:47AM)   Finished      4 / 4
  p03        (07/26/2025 03:49:47AM)   Finished      5 / 5
  p07        (07/26/2025 03:49:47AM)   Finished      6 / 6
  p08        (07/26/2025 03:49:47AM)   Finished      3 / 3
  p10        (07/26/2025 03:49:48AM)   Finished      5 / 5
──────────────────────────────────────────────────────────
 */


auto make_screen() -> CommandHandler {

  auto render_line = [](auto& process, auto state) -> str {
    auto& program = process.data.program;
    auto ip = program.ip;
    auto size = program.size();
    auto time = timestamp(process.data.stime);

    return format("  {:<10} {}   {}   {}\n",
      process.data.name,
      Text(format("({})", time))["fg33+pl"].get(),
      state,
      Text(format("{:>3} / {:<3}", ip, size))["fg208+pl"].get()
    );
  };

  auto process_queued = [](auto& name, auto& scheduler) -> bool {
    for (uint i = 0; i < 30; ++i) {
      if (scheduler.data.has_process(name))
        return true;

      with_unlocked([&] { 
        sleep_for(200ms); 
      });
    }
    
    return false;
  };

  return CommandHandler()
    .set_name("screen")
    .set_desc("Creates and switches through existing screens.")
    .set_min_args(0)
    .set_max_args(UINT_MAX)
    .add_flag("-s")
    .add_flag("-r")
    .add_flag("-ls")
    .add_flag("-c")
    .add_flag("-v")
    
    .set_validate([](Command& command, Shell& shell) -> optional<str> {
      auto has_ls = command.flags.contains("-ls");
      auto has_s = command.flags.contains("-s");
      auto has_r = command.flags.contains("-r");

      if (has_s + has_ls + has_r > 1)
        return "You must use only one of -s, -r, or -ls.";

      if (!shell.screen.is_main())
        return "Not in the Main Menu.";

      return nullopt;
    })

    .set_execute([&](Command& command, Shell& shell) {
      auto& screen = shell.screen;
      auto& scheduler = shell.scheduler;

      // === -ls: List screen info
      if (command.flags.contains("-ls")) {
        auto& data = scheduler.data;
        auto& cores = data.cores;

        auto size = cores.size();
        auto busy = cores.get_busy().size();
        auto cpu_util = cores.get_usage() * 100.0f;

        // Formatting
        auto line_width = 58u;
        auto blue = "fg33"s;

        cout << format("CPU Utilization: {:.2f}%\n", cpu_util);
        cout << format("Cores used: {} / {}\n\n", busy, size);
        cout << Text("─", line_width)[blue] << '\n';
        cout << "Running processes:\n";

        for (auto pid: cores.get_running_pids()) {
          auto& process = data.get_process(pid);
          auto core_id = process.data.core_id;
          cout << render_line(process, format("Core: {:<2}", core_id));
        }

        cout << "\nFinished processes:\n";
        for (auto pid: data.finished_pids) {
          auto& process = data.get_process(pid);
          cout << render_line(process, "Finished");
        }
        
        cout << Text("─", line_width)[blue];
        cout << '\n';
      }

      // === -s: Spawn and switch to new process screen
      else if (command.flags.contains("-s")) {
        if (command.args.size() < 2)
          return void(cout << "[screen] Usage: screen -s <name> <memory>\n");

        auto& name   = command.args[0];
        auto& memstr = command.args[1];

        if (scheduler.data.has_process(name))
          return void(cout << format("[screen] Process '{}' already exists.\n", name));

        // Parse memory argument
        // uint memory = stoui(memstr);
        // if (memory < 64 || memory > 65536 || (memory & (memory - 1)) != 0)
        //   return void(cout << "[screen] Invalid memory allocation. Must be power of 2 between 64 and 65536.\n");
        
        // Parse memory argument
        auto min_mem = scheduler.data.config.getu("min-mem-per-proc");
        auto max_mem = scheduler.data.config.getu("max-mem-per-proc");
        auto size = stoui(memstr);
        if (size < min_mem || size > max_mem || (size & (size - 1)) != 0)
          return void(cout << format("[screen] Invalid memory allocation. Must be power of 2 between {} and {}.\n", min_mem, max_mem));

        // Generate the process
        scheduler.generate_process(name, size);
        cout << format("[screen] Waiting for process creation: {}...", name);

        // Wait until queued
        if (!process_queued(name, scheduler))
          return void(cout << "\n[screen] Timed out.\n");

        // Retrieve process info
        auto& process = scheduler.data.get_process(name);
        auto pid = process.data.id;

        screen.switch_to(pid);
        cout << '\n';
        cout << format("Process name: {}\n", process.data.name);
        cout << format("ID: {}\n", pid);

        cout << "Logs:\n";
        for (auto& log : process.data.logs)
          cout << format("  {}\n", log);

        auto& program = process.data.program;
        cout << format("Current instruction line: {}\n", program.ip);
        cout << format("Lines of code: {}\n", program.size());
      }

      // === -r: Resume process by name
      else if (command.flags.contains("-r")) {
        
        if (command.args.empty()) 
          return void(cout << "Missing process name.\n");

        auto& name = command.args[0];
        auto& data = scheduler.data;

        if (!data.has_process(name))
          return void(cout << format("Process <{}> not found.\n", name));

        auto& process = data.get_process(name);
        auto pid = process.data.id;

        screen.switch_to(pid);
        cout << format("Process name: {}\n", process.data.name);
        cout << format("ID: {}\n", pid);

        cout << "Logs:\n";
        for (auto& log: process.data.logs)
          cout << format("  {}\n", log);

        cout << format("Current instruction line: {}\n", process.data.program.ip);
        cout << format("Lines of code: {}\n", process.data.program.size());
      }

      else if (command.flags.contains("-c")) {
        if (command.args.size() < 3)
          return void(cout << "[screen] Usage: screen -c <name> <memory> \"<instruction string>\"\n");

        auto& name = command.args[0];
        auto size = stoui(command.args[1]);
        auto& inst_str = command.args[2];

        // Check if process already exists
        if (scheduler.data.has_process(name))
          return void(cout << format("[screen] Process '{}' already exists.\n", name));

        // Validate memory size
        if (size < 64 || size > 65536 || (size & (size - 1)) != 0)
          return void(cout << "[screen] Invalid memory allocation. Must be power of 2 between 64 and 65536.\n");

        // Tokenize instruction string by semicolons
        auto token_lines = vec<vec<str>>();
        auto lines = re::split(inst_str, std::regex(";"));
        for (auto& line : lines) {
          if (!line.empty())
            token_lines.push_back(re::tokenize(re::strip(line)));
        }

        // Enforce instruction count limit
        if (token_lines.empty() || token_lines.size() > 50)
          return void(cout << "[screen] Instruction count must be between 1 and 50.\n");

        // Parse and validate instruction script
        auto& interpreter = InstructionInterpreter::get();
        auto script = interpreter.parse_script(token_lines);
        if (script.empty())
          return void(cout << "[screen] Failed to parse instruction script.\n");

        // Create process and memory view
        auto view = scheduler.data.memory.create_memory_view_for(0, size); // pid = 0 (will be set)
        auto process = Process(0, name, move(view), move(script));
        auto pid = scheduler.data.new_pid();
        process.data.id = pid;
        scheduler.data.add_process(move(process));
        scheduler.data.rqueue.push(pid);

        // Attach to screen
        screen.switch_to(pid);

        // Confirm to user
        auto& proc = scheduler.data.get_process(pid);
        cout << format("[screen] Process '{}' created with PID {}.\n", name, pid);
        cout << "Logs:\n";
        for (auto& log : proc.data.logs)
          cout << format("  {}\n", log);
        cout << format("Current instruction line: {}\n", proc.data.program.ip);
        cout << format("Lines of code: {}\n", proc.data.program.size());
      }

      else if (command.flags.contains("-v")) {
        if (command.args.empty())
          return void(cout << "[screen] Usage: screen -v <process_name>\n");

        auto& name = command.args[0];

        // Check if process exists
        if (!scheduler.data.has_process(name))
          return void(cout << format("[screen] Process '{}' not found.\n", name));

        auto& process = scheduler.data.get_process(name);
        auto& program = process.data.program;
        auto& memory  = process.data.memory;

        
        cout << format("[screen] Inspecting process '{}'\n", name);
        cout << "────────────────────────────────────────────\n";
        cout << format("{}\n", scheduler.data.memory.render_layout());
        cout << "Context Stack:\n" << program.render_context() << '\n';
        cout << "Instruction List:\n" << program.render_script() << '\n';
        cout << "Symbol Table:\n" << memory.render_symbol_table();
        cout << "────────────────────────────────────────────\n";
      }
    });
}
