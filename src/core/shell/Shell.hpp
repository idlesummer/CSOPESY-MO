#include "Shell.impl.hpp"
#include "core/command/handlers/_all.hpp"

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
      interpreter(Interpreter::instance()) {
    
    for (auto handler: command::get_all())
      interpreter.register_command(move(handler));
  }
}
