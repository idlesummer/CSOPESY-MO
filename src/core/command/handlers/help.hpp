#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/command/handlers/_all.hpp"
#include "core/common/utility/RichText.hpp"



auto make_help() -> CommandHandler {
  return CommandHandler()
    .set_name("help")
    .set_desc("Provides information of how the commands work")
    .set_min_args(0)
    .set_max_args(0)
    .set_disabled(false)
    
    .set_validate([](Command& command, Shell& shell) -> optional<str> {
      if (!shell.screen.is_main())
        return "Not in the Main Menu.";
      return nullopt;
    })

     .set_execute([](Command&, Shell& shell) {
      cout << RichText("  [b fg=#d39c6a]Available Commands: [/]\n\n");

      for (auto& [name, handler] : shell.interpreter.handlers) {
        if (handler.disabled) continue;
        // cout << format("  \033[1m{:<18}\033[0m {}\n", name, handler.desc);
        cout << RichText(format("  [bold]{:<18}[/] {}\n", name, handler.desc));
      }

      cout << "\nUse the command name directly to invoke it. Example: `exit`\n";
    });
}
