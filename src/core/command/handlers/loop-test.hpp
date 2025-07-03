#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/process/Process.hpp"
#include "core/process/ProcessProgram.hpp"
#include "core/process/ProcessExecutor.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"

namespace csopesy::command {

  inline const CommandHandler make_loop_test() {
    return {
      .name = "test",
      .desc = "Spawn and manually step a dummy FOR loop process.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},

      .execute = [](const Command&, Shell& shell) {
        using Instructions = ProcessProgram::list;

        system("cls");
        auto& storage = shell.get_storage();
        
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
          for (auto& inst : insts)
            program.add_instruction(move(inst));

          storage.set("test_proc", move(process));
          cout << "[loop-test] Process created.";
          return;
        }

        // 2. Access process
        auto& process = storage.get<Process>("test_proc");
        auto& program = process.get_program();
        auto& state   = process.get_state();

        if (state.is_finished()) {
          cout << "[loop-test] Process already finished.";
          return;
        }

        // === Debug: Show context stack ===
        const auto& ctx_stack = program.get_context();
        cout << "[loop-test] Context Stack:\n";
        if (ctx_stack.empty()) {
          cout << "  <empty>\n";

        } else {
          const auto& frames = ctx_stack.raw();
          for (uint i = 0; i < frames.size(); ++i) {
            const auto& frame = frames[i];
            cout << format(
              "  [{}] name: {}, line: {}, jump: {}, count: {}\n",
              i, frame.name, frame.line_addr, frame.jump_addr, frame.count
            );
          }
        }

        cout << '\n';

        // === Debug: Show instruction list with pointer ===
        const auto ip = program.get_ip();
        const auto& insts = program.get_instructions();

        cout << "[loop-test] Instruction List:\n";
        for (uint i = 0; i < insts.size(); ++i) {
          const auto& inst = insts[i];
          auto arrow = (i == ip) ? ">" : " ";
          cout << format("  {} [{:02}] {}", arrow, i, inst.opcode);
          for (const auto& arg : inst.args)
            cout << ' ' << arg;
          cout << '\n';
        }

        // 3. Step the process
        const bool done = ProcessExecutor::step(process.get_data());
        cout << "[loop-test] Process stepped.\n";
        cout << (done ? "Finished." : "Still running.");
      },

    };
  }
}
