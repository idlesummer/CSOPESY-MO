#pragma once
#include "core/common/imports/_all.hpp"
#include "types.hpp"

namespace csopesy {
  /**
   * @class Parser
   * @brief Utility class for tokenizing and parsing command lines.
   */
  class CommandParser {
    using list = Command::list;

    public:

    /** A helper that parses a handler line string into a Handler Command object. */
    static Command parse(const str& line) {
      auto tokens = tokenize(line);
      if (tokens.empty()) return {};

      auto cmd = Command();
      cmd.name = move(tokens[0]);

      for (uint i=1; i < tokens.size(); ++i) {
        str token = move(tokens[i]);

        if (token[0] != '-')
          cmd.args.emplace_back(move(token));

        else {
          bool next_is_arg = (i+1 < tokens.size() && tokens[i+1][0] != '-');
          cmd.flags.emplace(move(token), next_is_arg ? move(tokens[++i]) : "");
        }
      }

      return cmd;
    }

    private:
    
    /** A helper that splits a string into whitespace-separated tokens. */
    static list tokenize(const str& line) {
      if (line.empty()) return {};
      auto stream = isstream(line);
      list tokens; 

      for (str token; stream >> token; )
        tokens.emplace_back(move(token));
      return tokens;
    }
  };
}
