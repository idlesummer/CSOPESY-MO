#pragma once
#include "core/common/imports/_all.hpp"
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"


auto make_config() -> CommandHandler {
  return CommandHandler()
    .set_name("config")
    .set_desc("Displays the current scheduler configuration.")
    .set_min_args(0)
    .set_max_args(0)

    .set_execute([](Command&, Shell& shell) {
      auto& config = shell.scheduler.data.config;
      cout << "\n";
      cout << "╭──────────────────────── Scheduler Configuration ───────────────────────╮\n";
      cout << format("│ {:<22} │ {:<45} │\n", "Scheduler", config.scheduler);
      cout << format("│ {:<22} │ {:<45} │\n", "CPU Cores", config.num_cpu);
      cout << format("│ {:<22} │ {:<45} │\n", "Quantum Cycles", config.quantum_cycles);
      cout << format("│ {:<22} │ {:<45} │\n", "Batch Process Freq", config.batch_process_freq);
      cout << format("│ {:<22} │ {:<45} │\n", "Min Instructions", config.min_ins);
      cout << format("│ {:<22} │ {:<45} │\n", "Max Instructions", config.max_ins);
      cout << format("│ {:<22} │ {:<45} │\n", "Delays Per Exec", config.delays_per_exec);
      cout << format("│ {:<22} │ {:<45} │\n", "Initialized", config.initialized ? "Yes" : "No");
      cout << "╰────────────────────────────────────────────────────────────────────────╯\n";
    });
}
