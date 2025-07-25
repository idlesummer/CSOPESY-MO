#pragma once
#include "core/common/utility/Text.hpp"
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"


auto make_text() -> CommandHandler {
  return CommandHandler()
    .set_name("text")
    .set_desc("Print arguments.")
    .set_min_args(1)
    .set_max_args(UINT_MAX)
    .add_flag({ "-s", true })  // expects a value like: -s gr+b
    .set_disabled(false)

    .set_execute([](Command& command, Shell&) {
      auto style = "fg245"s;    // Default: soft gray
      if (command.flags.contains("-s"))
        style = command.flags.at("-s");

      auto out = osstream();
      for (auto& arg: command.args)
        out << arg << ' ';

      auto msg = out.str();
      if (!msg.empty())
        msg.pop_back();

      cout << Text(msg)[style] << '\n';
    });
}
