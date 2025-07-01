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
    mutable any context; ///< Optional shared state between `.validate` and `.execute`.
    
    /** Retrieves a reference to the stored context object of the given type. */
    template <typename Type>
    const Type& get_context() const {
      return any_cast<const Type&>(context);
    }
  };
}
