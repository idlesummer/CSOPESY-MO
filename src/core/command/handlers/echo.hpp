#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"


auto make_echo() -> CommandHandler {
  return CommandHandler()
    .set_name("echo")
    .set_desc("Print arguments.")
    .set_min_args(1)
    .set_max_args(UINT_MAX)
    .set_flags({})
    .set_disabled(false)

    .set_execute([](Command& command, Shell&) {
      for (auto& arg: command.args)
        cout << arg << ' ';
      cout << '\n';
    });
}
