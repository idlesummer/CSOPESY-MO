#pragma once
#include "core/common/imports/_all.hpp"
#include "core/shell/internal/Shell.impl.hpp"
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"


auto make_config() -> CommandHandler {
  return CommandHandler()
    .set_name("config")
    .set_desc("Displays the current scheduler configuration.")
    .set_min_args(0)
    .set_max_args(0)

    .set_execute([](Command&, Shell& shell) {
      auto& config = shell.scheduler.data.config;
      cout << "\n";
      cout << "╭──────────────────────── Scheduler Configuration ───────────────────────╮\n";

      for (auto& [key, val] : config.all()) {
        auto display = ""s;

        if (val.type() == typeid(str))
          display = format("\"{}\"", cast<str>(val));
        else if (val.type() == typeid(uint))
          display = to_string(cast<uint>(val));
        else if (val.type() == typeid(bool))
          display = cast<bool>(val) ? "true" : "false";
        else
          display = "<unsupported>";

        cout << format("│ {:<22} │ {:<45} │\n", key, display);
      }

      cout << "╰────────────────────────────────────────────────────────────────────────╯\n";
    });
}
