#pragma once
#include "core/common/imports/_all.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"

namespace csopesy::command {
  inline const CommandHandler make_cls() {
    return {
      .name = "cls",
      .desc = "Clear screen contents.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},
      .execute = [](Command&, Shell&) {
        system("cls");
      },
    };
  }
}
