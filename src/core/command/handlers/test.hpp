#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/process/Process.hpp"
#include "core/process/ProcessProgram.hpp"
#include "core/process/ProcessExecutor.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"

namespace csopesy::command {

  inline const CommandHandler make_test() {
    return {
      .name = "test",
      .desc = "Spawn and manually step a dummy FOR loop process.",
      .min_args = 0,
      .max_args = 0,
      .flags = {{ "--reset", false }},

      .execute = [](const Command& command, Shell& shell) {
        using Instructions = ProcessProgram::list;
        
        auto& storage = shell.get_storage();

        // 0. Reset process if requested
        if (command.flags.contains("--reset")) {
          shell.get_storage().remove("test_proc");
          cout << "[loop-test] Dummy process reset.\n";
          return;
        }
        
        // 1. Spawn process if not already present
        if (!storage.has("test_proc")) {
          auto insts = Instructions{
            { "PRINT", { "Start!" }},
            { "FOR", { "3" }},
            { "PRINT", { "outer loop" }},
            { "FOR", { "2" }},
            { "PRINT", { "inner loop" }},
            { "ENDFOR" },
            { "ENDFOR" },
            { "PRINT", { "Done!" }},
          };

          auto process = Process("loop-test", 999);
          auto& program = process.get_program();
          for (auto& inst: insts)
            program.add_instruction(move(inst));

          storage.set("test_proc", move(process));
          cout << "[loop-test] Process created.\n";
          return;
        }

        // 2. Access process
        system("cls");
        auto& process = storage.get<Process>("test_proc");
        auto& program = process.get_program();
        auto& state   = process.get_state();

        if (state.is_finished())
          return void(cout << "[loop-test] Process already finished.");

        // === Debug: Show context stack ===
        cout << "[loop-test] Context Stack:\n";
        cout << program.view_context() << '\n';

        // === Debug: Show instruction list with pointer ===
        cout << "[loop-test] Instruction List:\n";
        cout << program.view_instructions() << '\n';
        
        // 3. Step the process
        const bool done = ProcessExecutor::step(process.get_data());
        cout << "[loop-test] Process stepped.\n";
        cout << (done ? "Finished." : "Still running.");
        cout << '\n';
      },
    };
  }
}
