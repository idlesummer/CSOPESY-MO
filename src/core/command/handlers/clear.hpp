#pragma once
#include "core/common/imports/_all.hpp"
#include "core/command/types.hpp"

namespace csopesy::command {
  inline const CommandHandler make_clear() {
    return {
      .name = "clear",
      .desc = "Clear screen contents.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},
      .execute = [](const Command&, Shell&) {
        system("cls");
      },
    };
  }
}
