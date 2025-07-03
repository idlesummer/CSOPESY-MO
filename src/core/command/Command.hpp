#pragma once 
#include "core/common/imports/_all.hpp"

namespace csopesy {

  /** Represents a parsed command-line input. */
  struct Command {
    using list = vector<str>;  
    using map = unordered_map<str, str>;
    str  name;    ///< The command name (e.g., "screen", "exit")
    list args;    ///< Positional arguments for the command
    map  flags;   ///< Map of flags and their associated values (if any)
  };
}
