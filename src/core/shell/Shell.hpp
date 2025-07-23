#include "internal/Shell.impl.hpp"        // Must be included first to fully define Shell
#include "core/command/handlers/_all.hpp" // Must be included after since handlers use Shell&
#include "core/common/utility/EventEmitter.hpp"

  
/**
 * @brief Populates the CommandInterpreter with all available commands.
 * 
 * Defined separately so the Shell class is fully known before
 * including handlers that use Shell&.
 */
void Shell::register_commands() {
  for (auto handler: get_command_handlers())
    interpreter.register_command(move(handler));
}
