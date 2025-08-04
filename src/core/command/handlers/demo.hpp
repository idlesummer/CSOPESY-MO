#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionInterpreter.hpp"
#include "core/process/Process.hpp"
#include "core/process/ProcessProgram.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"

auto make_demo() -> CommandHandler {
  return CommandHandler()
    .set_name("demo")
    .set_desc("Spawn and manually step a dummy FOR loop process.")
    .set_min_args(0)
    .set_max_args(0)
    .add_flag("-r")
    .set_execute([](Command& command, Shell& shell) {
      auto& interpreter = InstructionInterpreter::get();
      auto& storage = shell.storage;

      // Reset demo process
      if (command.flags.contains("-r")) {
        shell.storage.remove("demo.process");
        cout << "[demo] Dummy process reset.\n";
        return;
      }

      // Create process if not yet spawned
      if (!storage.has("demo.process")) {
        auto view = shell.scheduler.data.memory.create_memory_view_for(0, 64);
        auto process = Process(0, "demo.process", 10, move(view));
        storage.set("demo.process", move(process));
        cout << "[demo] Process created.\n";
      }

      // Access process
      auto& process = storage.get<Process>("demo.process");
      auto& program = process.data.program;

      // Show context
      cout << "[demo] Context Stack:\n" << program.render_context() << '\n';

      // Show instruction list
      cout << "[demo] Instruction List:\n" << program.render_script() << '\n';

      // Show symbol table and their current values
      cout << "[demo] Symbol Table:\n";
      for (auto& [var, vaddr] : process.data.memory.symbol_table) {
        auto [value, is_violation, is_page_fault] = process.data.memory.virtual_memory.read(vaddr);
        cout << format("  {:<6} → vaddr={:#04x} = {:<5}", var, vaddr, value);
        if (is_violation) cout << "  [VIOLATION]";
        if (is_page_fault) cout << "  [PAGE FAULT]";
        cout << '\n';
      }
      cout << '\n';

      // Check if process is finished before stepping
      if (program.finished()) {
        cout << "[demo] Process already finished.\n";
        return;
      
      } else {
        // Step
        auto done = process.step();
        cout << "[demo] Process stepped.\n";
        cout << (done ? "Finished." : "Still running.") << '\n';
      }
    });
}
