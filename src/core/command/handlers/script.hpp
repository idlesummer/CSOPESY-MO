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

      // Reset script process
      if (command.flags.contains("-r")) {
        storage.remove("script.process");
        cout << "[script] Script process reset.\n";
        return;
      }

      // ───────────────────────────────────────
      // Create and store script process if not yet existing
      // ───────────────────────────────────────
      if (!storage.has("script.process")) {
        auto token_lines = vec<vec<str>>{
          { "DECLARE", "x", "0xF"       },
          { "DECLARE", "y", "10"        },
          { "ADD",     "sum", "x", "y"  },
          { "PRINT",   "sum"            },
          { "DECLARE", "hexnum", "0x1F4" },
          { "ADD",     "sum2", "hexnum", "0x10" },
          { "PRINT",   "sum2" }
        };

        auto script = interpreter.parse_script(token_lines);
        if (script.empty()) {
          cout << "[script] Failed to parse script.\n";
          return;
        }

        auto view = shell.scheduler.data.memory.create_memory_view_for(0, 64);
        auto process = Process(0, "script", move(view), move(script));
        storage.set("script.process", move(process));

        cout << "[script] Script process created.\n";
      }

      // ───────────────────────────────────────
      // Access and step the process
      // ───────────────────────────────────────
      auto& process = storage.get<Process>("script.process");
      auto& program = process.data.program;
      auto& memory  = process.data.memory;

      cout << "[script] ================================\n";
      cout << "[script] Script instructions:\n";
      cout << program.render_script() << '\n';

      cout << "[script] Symbol Table:\n";
      cout << memory.render_symbol_table() << '\n';

      if (program.finished())
        cout << "[script] Already finished.\n";

      else {
        process.step();
        cout << "[script] Stepped once.\n";
      }

      for (auto& log : process.data.logs)
        cout << format("{}\n", log);

      cout << "[script] ================================\n";
    });
}
