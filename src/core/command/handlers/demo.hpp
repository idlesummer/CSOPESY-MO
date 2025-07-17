#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/process/Process.hpp"
#include "core/instruction/InstructionInterpreter.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"


inline const CommandHandler make_demo() {
  return {
    .name = "demo",
    .desc = "Creates and steps a demo process with random instructions.",
    .min_args = 0,
    .max_args = 0,

    .execute = [](Command&, Shell& shell) {
      auto& storage = shell.storage;
      
      if (!shell.storage.has("demo.counter")) {
        cout << "Counter created! Run demo again to increment counter.\n";
        cout << "Counter: 0\n";
        storage.set("demo.counter", 0);
        return;
      }

      int counter = storage.get<int>("demo.counter") + 1;
      storage.set("demo.counter", counter);
      cout << format("Counter: {}\n", counter);;
    },
  };
}
