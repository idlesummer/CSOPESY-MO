#include "internal/Shell.impl.hpp"        // Must be included first to fully define Shell
#include "core/command/handlers/_all.hpp" // Must be included after since handlers use Shell&

  
/**
 * @brief Shell constructor with command registration.
 * 
 * Defined separately so the Shell class is fully known before
 * including handlers that use Shell&. This constructor populates
 * the CommandInterpreter with all available commands.
 */
Shell::Shell(EventEmitter& emitter): 
    global      (emitter),                    // Reference to the global EventEmitter
    interpreter (CommandInterpreter::get()),  // Instance of the command interpreter
    thread      (),                           // Shell thread (starts later in start())
    active      (atomic_bool{true}),          // Atomic flag to control shell lifecycle
    scheduler   (Scheduler()),                // Owned scheduler instance
    storage     (ShellStorage()),             // Storage for dynamic shell/session data
    screen      (storage)                     // Owned instance of shell screen manager
{  
  // Register all command handlers
  for (auto handler: get_command_handlers())
    interpreter.register_command(move(handler));
}
