#pragma once
#include "core/common/imports/_all.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"

auto make_script() -> CommandHandler {
  return CommandHandler()
    .set_name("script")
    .set_desc("Parse and step through a hardcoded instruction script.")
    .set_min_args(0)
    .set_max_args(0)
    .set_execute([](Command& command, Shell& shell) {
      auto& interpreter = InstructionInterpreter::get();
      auto& storage     = shell.storage;

      // ───────────────────────────────────────
      // Hardcoded tokenized script
      // ───────────────────────────────────────
      auto token_lines = vec<vec<str>>{
        { "DECLARE", "x", "10" },
        { "ADD",     "x", "1",  "2" },
        { "PRINT",   "x" },
        { "DECLARE", "y", "0xA" },      // 10 in hex
        { "ADD",     "y", "0x1", "x" }, // y = 1 + x
        { "PRINT",   "y" },
      };

      // ───────────────────────────────────────
      // Parse the instruction script
      // ───────────────────────────────────────
      auto script = interpreter.parse_script(token_lines);
      if (script.empty())
        return void(cout << "[script] Failed to parse script.\n");

      // ───────────────────────────────────────
      // Create and initialize dummy process
      // ───────────────────────────────────────
      auto view    = shell.scheduler.data.memory.create_memory_view_for(0, 64);
      auto process = Process(0, "script", move(view), move(script));
      auto& program = process.data.program;

      // ───────────────────────────────────────
      // Display the parsed script
      // ───────────────────────────────────────
      cout << "[script] Instructions to execute:\n";
      cout << program.render_script() << '\n';

      // ───────────────────────────────────────
      // Step through and execute the script
      // ───────────────────────────────────────
      cout << "[script] Execution output:\n";
      cout << "────────────────────────────────────────\n";

      while (!program.finished())
        process.step();

      for (auto& log : process.data.logs)
        cout << format("{}\n", log);
      cout << "────────────────────────────────────────\n";
      cout << "[script] Script execution complete.\n";
    });
}
