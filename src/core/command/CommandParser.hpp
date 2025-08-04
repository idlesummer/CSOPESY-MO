#pragma once
#include "core/common/imports/_all.hpp"
#include "core/command/Command.hpp"


/**
 * @class CommandParser
 * @brief Utility class for tokenizing and parsing command lines.
 */
class CommandParser {
  public:

  /** @brief Parses a line into a Command object (name, args, flags, etc.). */
  static auto parse(str& line) -> Command {
    auto tokens = re::tokenize(re::strip(line));
    if (tokens.empty()) return {};

    auto cmd = Command();
    cmd.name = re::strip(tokens[0]);

    for (auto i = 1u; i < tokens.size(); ++i) {
      auto token = re::strip(tokens[i]);

      if (token.empty()) continue;  // skip empty tokens just in case
      cmd.tokens.push_back(token);  // store all meaningful tokens after name

      if (token[0] == '-')
        cmd.flags.insert(token);
      else
        cmd.args.push_back(token);
    }

    // Save input string (everything after the command name)
    auto pos = line.find_first_not_of(' ', cmd.name.size());
    cmd.input = (pos != npos) ? re::strip(line.substr(pos)) : "";
    return cmd;
  }
};
