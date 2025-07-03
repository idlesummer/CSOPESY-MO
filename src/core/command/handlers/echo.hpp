#pragma once
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"

namespace csopesy::command {
  inline const CommandHandler make_echo() {
    return {
      .name = "echo",
      .desc = "Print arguments.",
      .min_args = 1,
      .max_args = MAX,
      .flags = {},
      
      .execute = [](const Command& command, Shell&) {
        for (const auto& arg: command.args)
          cout << arg << ' ';
      },
    };
  }
}
