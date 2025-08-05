#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"


auto make_vmstat() -> CommandHandler {
  return CommandHandler()
    .set_name("vmstat")
    .set_desc("Provides a detailed view of the active/inactive processes, available/used memory, and pages.")
    .set_min_args(0)
    .set_max_args(0)
    
    .set_validate([](Command& command, Shell& shell) -> optional<str> {
      if (!shell.screen.is_main())
        return "Not in the Main Menu.";

      return nullopt;
    })

    .set_execute([](Command& command, Shell& shell) {
      auto& scheduler = shell.scheduler;
      auto& memory    = scheduler.data.memory;
      auto& ticks = scheduler.ticks;

      auto total_mem = memory.get_total_memory();
      auto free_mem  = memory.get_free_memory();
      auto used_mem  = memory.get_used_memory();

      cout << format("Total memory     : {:>8} bytes\n", total_mem);
      cout << format("Used memory      : {:>8} bytes\n", used_mem);
      cout << format("Free memory      : {:>8} bytes\n", free_mem);
      cout << format("CPU total ticks  : {:>8}\n", ticks);
      // cout << "\n";
      // cout << format("Pages paged in   : {:>8}\n", paged_in);
      // cout << format("Pages paged out  : {:>8}\n", paged_out);
    });
}
