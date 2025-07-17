#pragma once 
#include "core/common/imports/_all.hpp"
#include "Command.hpp"
#include "types.hpp"


/** Represents a single command entry in the interpreter. */
class Shell;
class CommandHandler {
  public:
    CommandHandler():
      name     (""s),         // Command name (e.g., "screen", "exit")
      desc     (""s),         // Description for help or documentation
      min_args (0u),          // Minimum number of required arguments
      max_args (UINT_MAX),    // Maximum number of allowed arguments
      flags    (vec<Flag>()), // Valid flags for this command
      validate (nullptr),     // Optional validation hook
      execute  (nullptr) {}   // Main handler for executing the command

    // === Chainable Setters ===
    auto set_name(const str& value) -> CommandHandler& { return name = value, *this; }
    auto set_desc(const str& value) -> CommandHandler& { return desc = value, *this; }
    auto set_min_args(uint value) -> CommandHandler& { return min_args = value, *this; }
    auto set_max_args(uint value) -> CommandHandler& { return max_args = value, *this; }
    auto set_flags(const vec<Flag>& value) -> CommandHandler& { return flags = value, *this; }
    auto set_validate(func<optional<str>(Command&, Shell&)> value) -> CommandHandler& { return validate = value, *this; }
    auto set_execute(func<void(Command&, Shell&)> value) -> CommandHandler& { return execute = value, *this; }

      
    // === Member variables === 
    str name;                        
    str desc;                        
    uint min_args;                
    uint max_args;              
    vec<Flag> flags;                       
    func<optional<str>(Command&,Shell&)> validate; 
    func<void(Command&,Shell&)> execute;
};
