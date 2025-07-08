#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/process/Process.hpp"
#include "core/instruction/InstructionInterpreter.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"

namespace csopesy::command {
  inline const CommandHandler make_demo() {
    return {
      .name = "demo",
      .desc = "Creates and steps a demo process with random instructions.",
      .min_args = 0,
      .max_args = 0,

      .execute = [](const Command&, Shell& shell) {
        auto& storage = shell.get_storage();
        
        if (!storage.has("demo.counter")) {
          storage.set("demo.counter", 0);
          return;
        }

        int counter = storage.get<int>("demo.counter");
        counter++;
        storage.set("demo.counter", counter);
        cout << counter << endl;
      },
    };
  }
}
