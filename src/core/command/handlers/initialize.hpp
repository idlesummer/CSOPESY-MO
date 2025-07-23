#pragma once
#include "core/common/imports/_all.hpp"
#include "core/common/constants/banner.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"
#include "core/scheduler/types.hpp"


auto make_initialize() -> CommandHandler {
  return CommandHandler()
    .set_name("initialize")
    .set_desc("Initializes the processor configuration of the application.")
    .set_min_args(0)
    .set_max_args(0)
    .set_flags({})
    .set_disabled(false)

    .set_validate([](Command& command, Shell& shell) -> Str {
      auto config = shell.scheduler.data.config;
      
      // Check if the scheduler has already been initialized
      if (config.getb("initialized"))
        return "Already initialized.";

      // Check if the file could not be opened or is empty
      auto lines = read_lines("config.txt");
      if (lines.empty())
        return "Failed to open config.txt";

      auto& storage = shell.storage;
      storage.set("initialize.cache", move(lines));
      return nullopt;
    })

    .set_execute([](Command& command, Shell& shell) {
      auto& storage = shell.storage;
      auto& lines = storage.get<vec<str>>("initialize.cache");
      auto config = Config();

      for (auto& line: lines) {
        str key, value;
        isstream(line) >> key >> value;
        config.set(key, Config::parse(value));
      }

      config.set("initialized", true);
      shell.scheduler.set_config(move(config));

      system("cls");
      cout << format("{}\n", BANNER);
      cout << "[Shell] Scheduler config loaded.\n";
      storage.remove("initialize.cache");

      // Enable other commands
      for (auto& [name, handler]: shell.interpreter.handlers)
        handler.set_disabled(false);
    });
}
