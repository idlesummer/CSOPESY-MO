#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"

namespace csopesy::command {
  inline const CommandHandler make_exit() {
    return {
      .name = "exit",
      .desc = "Exit shell.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},
      
      .execute = [](const Command&, Shell& shell) {
        auto& screen = shell.get_screen();

        if (screen.is_main()) {
          shell.request_stop();
          shell.emit("shutdown");

        } else {
          screen.switch_to_main();
          shell.emit("switched_to_main");
        }
      },
    };
  }
}
