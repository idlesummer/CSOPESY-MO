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

  auto process_exists = [](auto& name, auto& scheduler) -> bool {
    auto& data = scheduler.data;
    return data.has_process(name);
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
    .add_flag({ "-s", true })
    .add_flag({ "-r", true })
    .add_flag({ "-ls", false })
    .add_flag({ "-c", false })
    
    .set_validate([](Command& command, Shell& shell) -> Str {
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
      }

      // === -s: Spawn and switch to new process screen
      else if (command.flags.contains("-s")) {
        auto& name = command.flags.at("-s");

        if (process_exists(name, scheduler))
          return void(cout << format("Process '{}' already exists\n", name));

        scheduler.generate_process(name);
        cout << format("\nWaiting for process creation: {}", name);

        // Wait until process queues
        if (!process_queued(name, scheduler))
          return void(cout << "\nTimed out.\n");

        auto& process = scheduler.data.get_process(name);
        auto pid = process.data.id;

        screen.switch_to(pid);
        cout << '\n';
        cout << format("Process name: {}\n", process.data.name);
        cout << format("ID: {}\n", pid);

        cout << "Logs:\n";
        for (auto& log: process.data.logs)
          cout << format("  {}\n", log);

        auto& program = process.data.program;
        cout << format("Current instruction line: {}\n", program.ip);
        cout << format("Lines of code: {}\n", program.size());
      }

      // === -r: Resume process by name
      else if (command.flags.contains("-r")) {
        auto& name = command.flags.at("-r");
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
        for (auto& arg: command.args)
          cout << format("{}\n", arg);
      }
    });
}
