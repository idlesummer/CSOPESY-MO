#pragma once 
#include "core/common/imports/_all.hpp"
#include "Command.hpp"
#include "types.hpp"


/** Represents a single command entry in the interpreter. */
class Shell;
struct CommandHandler {
  using list = vector<CommandFlag>;
  using execute_func  = function<void(Command&, Shell&)>;
  using validate_func = function<Str(Command&, Shell&)>;

  str  name;                        // Command name (e.g., "screen", "exit")
  str  desc;                        // Description for help or documentation
  uint min_args = 0;                // Minimum number of required arguments
  uint max_args = MAX;              // Maximum number of allowed arguments
  list flags;                       // Valid flags for this command
  validate_func validate = nullptr; // Optional validation hook
  execute_func  execute;            // Main handler for executing the command
};
