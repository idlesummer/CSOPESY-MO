#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionInterpreter.hpp"
#include "core/process/Process.hpp"
#include "core/process/ProcessProgram.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"

namespace csopesy::command {
  inline const CommandHandler make_test() {
    return {
      .name = "test",
      .desc = "Spawn and manually step a dummy FOR loop process.",
      .min_args = 0,
      .max_args = 0,
      .flags = {{ "-r", false }},

      .execute = [](const Command& command, Shell& shell) {
        using Interpreter = InstructionInterpreter;
        
        auto& interpreter = Interpreter::instance();
        auto& storage = shell.storage;

        // 0. Reset process if requested
        if (command.flags.contains("-r")) {
          shell.storage.remove("test.process");
          cout << "[loop-test] Dummy process reset.\n";
          return;
        }

        // 1. Spawn process if not already present
        if (!storage.has("test.process")) {
          auto process = Process::create(999, "test.process", 10);
          storage.set("test.process", move(process));
          cout << "[loop-test] Process created.\n";
          return;
        }

        // 2. Access process
        system("cls");
        auto& process = storage.get<Process>("test.process");
        auto& program = process.data.program;
        auto& state = process.data.state;

        if (state.finished())
          return void(cout << "[loop-test] Process already finished.");

        // === Debug: Show context stack ===
        cout << "[loop-test] Context Stack:\n";
        cout << program.render_context() << '\n';

        // === Debug: Show instruction list with pointer ===
        cout << "[loop-test] Instruction List:\n";
        cout << program.render_script() << '\n';
        
        // 3. Step the process
        const bool done = process.step();
        cout << "[loop-test] Process stepped.\n";
        cout << (done ? "Finished." : "Still running.");
        cout << '\n';
      },
    };
  }
}
