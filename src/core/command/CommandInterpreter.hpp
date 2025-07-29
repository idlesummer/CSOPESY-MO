#pragma once
#include "core/common/imports/_all.hpp"
#include "CommandHandler.hpp"
#include "CommandParser.hpp"


/**
 * @class CommandInterpreter
 * @brief Responsible for parsing, validating, and executing registered commands.
 * Acts as a central dispatcher for handling parsed input lines and invoking the appropriate command logic.
 */
class Shell;  // Forward declaration
class CommandInterpreter {
  public: 

  CommandInterpreter():
    handlers (map<str,CommandHandler>()) {} // Registry of available command handlers
  
  /** @brief Returns the global singleton get of the CommandInterpreter. */
  static CommandInterpreter& get() {
    static auto inst = CommandInterpreter();
    return inst;
  }

  /** @brief Registers a command with its name and handlers. */
  void register_command(CommandHandler handler) {
    handlers[handler.name] = move(handler);
  }

  /** @brief Executes a command in the shell context. */ 
  void execute(const str& line, Shell& shell) {

    // Parse the command line into a Command (name, args, flags)
    auto command = CommandParser::parse(line);  
    if (command.name.empty()) 
      return;

    // Find command in the registry
    auto it = handlers.find(command.name);       
    if (it == handlers.end())
      return void(cout << format("[Shell] Unknown command: {}\n", command.name));

    auto& handler = it->second;

    // Check if command is disabled
    if (handler.disabled)
      return void(cout << "[Shell] This command is unavailable.\n");

    // Check for unknown/misused flags
    if (invalid_flags(command, handler))
      return void(cout << format("[Shell] Invalid or misused flag(s) for '{}'\n", command.name));

    // Check positional argument count
    if (invalid_args(command, handler))
      return void(cout << format("[Shell] Invalid number of arguments for '{}'\n", command.name));                  

    // Run optional command-specific validation logic
    if (auto msg = custom_validation(command, handler, shell))
      return void(cout << format("[Shell] {}\n", *msg));
    
    // Execute the command
    cout << '\n';
    handler.execute(command, shell);
  }

  // ------- Instance variables -------
  
  map<str,CommandHandler> handlers; 

  // ------- Internal logic -------

  private:

  /** @brief Returns true if any flag is invalid or misused. */
  static auto invalid_flags(Command& command, CommandHandler& handler) -> bool {
    // If any user flag has no valid match (in name and usage), then the set of flags is invalid.
    return false;
    return any_of(command.flags, [&](auto& flag) {
      return none_of(handler.flags, [&](Flag& f) {
        return f.name == flag.first && f.has_arg == !flag.second.empty();
      });
    });
  }

  /** @brief Checks if argument count is within bounds. */
  static auto invalid_args(Command& command, CommandHandler& handler) -> bool {
    auto argc = command.args.size();
    return argc < handler.min_args || argc > handler.max_args;
  }

  /** Runs the handler's custom validator and returns an optional error message. */
  static auto custom_validation(Command& command, CommandHandler& handler, Shell& shell) -> Str {
    if (!handler.validate) return nullopt;
    if (auto err = handler.validate(command, shell))
      return err->empty() ? "CommandHandler disabled or invalid." : *err;
    return nullopt;
  }
};
