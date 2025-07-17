#pragma once 
#include "core/common/imports/_all.hpp"


/** @brief Represents a parsed command-line input. */
class Command {
  public:
    Command():
      name  (""s),              // The command name (e.g., "screen", "exit")
      args  (vec<str>()),       // Positional arguments for the command
      flags (map<str,str>()) {} // Map of flags and their associated values (if any)


    // === Member variables ===
    str name;    
    vec<str> args;    
    map<str,str> flags;   
};
