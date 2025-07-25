#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"


auto make_exit() -> CommandHandler {
  return CommandHandler()
    .set_name("exit")
    .set_desc("Exit shell.")
    .set_min_args(0)
    .set_max_args(0)
    .set_disabled(false)

    .set_execute([](Command&, Shell& shell) {
      auto& screen = shell.screen;
      if (screen.is_main())
        return void(shell.stop());

      screen.switch_to_main();
      cout << shell.banner.get();
    });
}
