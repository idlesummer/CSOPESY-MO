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
  
  
  /** Contains the flag name and whether it requires an argument. */
  struct CommandFlag {
    str  name;              ///< Flag name (e.g., "-s", "-r", "-ls")
    bool has_arg = false;   ///< Whether the flag requires an argument
  };
  
  
  /** Represents a single command entry in the interpreter. */
  class Shell;
  struct CommandHandler {
    using list = vector<CommandFlag>;
    using execute_func  = function<void(const Command&, Shell&)>;
    using validate_func = function<Str(const Command&, Shell&)>;
    
    str  name;                        ///< Command name (e.g., "screen", "exit")
    str  desc;                        ///< Description for help or documentation
    uint min_args = 0;                ///< Minimum number of required arguments
    uint max_args = MAX;        ///< Maximum number of allowed arguments
    list flags;                       ///< Valid flags for this command
    validate_func validate = nullptr; ///< Optional validation hook
    execute_func  execute;            ///< Main handler for executing the command
  };
}
