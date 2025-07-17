#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"


auto make_demo() -> CommandHandler {
  return CommandHandler()
    .set_name("demo")
    .set_desc("Creates and steps a demo process with random instructions.")
    .set_min_args(0)
    .set_max_args(0)
    
    .set_execute([](Command&, Shell& shell) {
      auto& storage = shell.storage;

      if (!storage.has("demo.counter")) {
        cout << "Counter created! Run demo again to increment counter.\n";
        cout << "Counter: 0\n";
        storage.set("demo.counter", 0u);
        return;
      }

      auto counter = storage.get<uint>("demo.counter") + 1;
      storage.set("demo.counter", counter);
      cout << format("Counter: {}\n", counter);
    });
}
