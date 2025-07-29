#pragma once 
#include "core/common/imports/_all.hpp"


/** 
 * @brief Represents a parsed command-line input.
 * 
 * A Command encapsulates all components of a parsed input string,
 * including the command name, original argument input (excluding the command),
 * tokenized parts, positional arguments, and named flags.
 */
class Command {
  public:

  Command():
    name   (""s),           // The command name (e.g., "screen", "exit")
    input  (""s),           // Original input string after the command name
    tokens (vec<str>()),    // All space-delimited tokens after the command name
    args   (vec<str>()),    // Positional arguments for the command
    flags  (set<str>()) {}  // Set of flags (automatically deduplicated)

  // ------- Instance variables -------

  str name;
  str input;
  vec<str> tokens;
  vec<str> args;
  set<str> flags;
};
