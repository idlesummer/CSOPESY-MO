#pragma once
#include "core/common/utility/RichText.hpp"
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"


auto make_echo() -> CommandHandler {
  return CommandHandler()
    .set_name("echo")
    .set_desc("Print arguments.")
    .add_flag("--rich")
    .set_min_args(1)
    .set_max_args(UINT_MAX)
    .set_disabled(false)

    .set_execute([](Command& command, Shell&) {
      if (!command.flags.contains("--rich"))
        cout << RichText(join(command.args)) << '\n';
      else 
        cout << join(command.args) << '\n';
    });
}
