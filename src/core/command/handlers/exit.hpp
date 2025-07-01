#pragma once
#include "core/shell/Shell.hpp"
#include "core/command/types.hpp"

namespace csopesy::command {
  inline const CommandHandler make_exit() {
    return {
      .name = "exit",
      .desc = "Exit shell.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},
      .execute = [](const Command&, Shell& shell) {
        if (shell.get_screen() == Screen::MAIN_MENU) {
            shell.request_stop();
            shell.emit("shutdown");
          } else {
            shell.switch_screen(Screen::MAIN_MENU);
            shell.emit("switched_to_main");
          }
      },
    };
  }
}
