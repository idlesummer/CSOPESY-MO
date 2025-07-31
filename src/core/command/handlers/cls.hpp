#pragma once
#include "core/common/imports/_all.hpp"
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"


auto make_cls() -> CommandHandler {
  return CommandHandler()
    .set_name("cls")
    .set_desc("Clear screen contents.")
    .set_min_args(0)
    .set_max_args(0)
    .set_disabled(false)

    .set_validate([](Command& command, Shell& shell) -> Str {
       auto config = shell.scheduler.data.config;
      if (!config.getb("initialized"))
        return "Scheduler not initialized. Please run 'initialize' first.";

      return nullopt;
    })

    .set_execute([](Command&, Shell& shell) {
      system("cls");
      cout << shell.banner.get();
    });
}

