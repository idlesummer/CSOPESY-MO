#include "internal/Shell.impl.hpp"        // Must be included first to fully define Shell
#include "core/command/handlers/_all.hpp" // Must be included after since handlers use Shell&

namespace csopesy {
  
  /**
   * @brief Shell constructor with command registration.
   * 
   * Defined separately so the Shell class is fully known before
   * including handlers that use Shell&. This constructor populates
   * the CommandInterpreter with all available commands.
   */
  inline Shell::Shell(EventEmitter& emitter): 
      Component(emitter), 
      interpreter(Interpreter::instance()),
      screen(storage) {
    
    // Register all command handlers
    for (auto handler: command::get_all())
      interpreter.register_command(move(handler));
  }
}
