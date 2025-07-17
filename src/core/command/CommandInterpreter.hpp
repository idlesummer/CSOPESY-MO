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
  using Flag = CommandFlag;
  using Handler = CommandHandler;
  using map = unordered_map<str, Handler>;
  using set = unordered_set<str>;
  
  public:
  map handlers; // Registry of available command handlers
  
  // === Methods ===

  /** @brief Returns the global singleton instance of the CommandInterpreter. */
  static CommandInterpreter& instance() {
    static CommandInterpreter inst;
    return inst;
  }

  /** @brief Registers a command with its name and handlers. */
  void register_command(Handler handler) {
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


  // ========================
  // === Private Helpers ====
  // ========================
  private:


  /** @brief Private constructor to enforce singleton access via instance(). */
  CommandInterpreter() = default;
  
  /** @brief Returns true if any flag is invalid or misused. */
  static bool invalid_flags(Command& command, Handler& handler) {
    // If any user flag has no valid match (in name and usage), 
    // then the set of flags is invalid.
    return any_of(command.flags, [&](auto& flag) {
      return none_of(handler.flags, [&](Flag& f) {
        return f.name == flag.first && f.has_arg == !flag.second.empty();
      });
    });
  }

  /** @brief Checks if argument count is within bounds. */
  static bool invalid_args(Command& command, Handler& handler) {
    auto argc = command.args.size();
    return argc < handler.min_args || argc > handler.max_args;
  }

  /** Runs the handler's custom validator and returns an optional error message. */
  static Str custom_validation(Command& command, Handler& handler, Shell& shell) {
    if (!handler.validate) return nullopt;
    if (auto err = handler.validate(command, shell))
      return err->empty() ? "Handler disabled or invalid." : *err;
    return nullopt;
  }
};
