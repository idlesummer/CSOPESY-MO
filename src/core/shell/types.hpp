#pragma once

namespace csopesy {

  /** Represents the different screen modes available in the OS emulator shell. */
  enum class Screen {
    MAIN_MENU,    ///< Main CLI screen for global commands and process management
    PROCESS_VIEW, ///< Active screen tied to a specific process
  };
}
