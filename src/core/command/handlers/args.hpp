#pragma once
#include "core/common/imports/_all.hpp"
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"


auto make_args() -> CommandHandler {
  return CommandHandler()
    .set_name("args")
    .set_desc("Debug how CommandParser splits command args and flags.")
    .set_min_args(0)
    .set_disabled(false)

    .set_execute([](Command& command, Shell& shell) {
      cout << format("Command name: {}\n", command.name);
      cout << format("Raw input:    {}\n", command.input);

      cout << "Tokens:\n";
      for (auto& tok : command.tokens)
        cout << format("  [{}]\n", tok);

      cout << "\nArgs:\n";
      for (auto& arg : command.args)
        cout << format("  [{}]\n", arg);

      cout << "\nFlags:\n";
      for (auto& flag : command.flags)
        cout << format("  [{}]\n", flag);
      cout << '\n';
    });
}
