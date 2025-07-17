#pragma once
#include "core/shell/ShellStorage.hpp"
#include "core/common/constants/banner.hpp"


class ShellScreen {  
  public:
  
  /** Initializes the screen and defaults to the main screen (ID 0). */
  ShellScreen(ShellStorage& storage): 
    storage(storage) 
  {
    storage.set("screen.id", 0u);
  }

  /** Switch to a screen by ID. Use "main" or "" for the main screen. */
  void switch_to(uint id) {
    storage.set("screen.id", id);
    if (is_main()) cout << BANNER << '\n';
  }

  /** Switch to the main screen (empty ID). */
  void switch_to_main() { switch_to(0); }

  /** Returns the screen ID string. */
  auto get_id() -> uint const { return storage.get<uint>("screen.id"); }

  /** Helper: Check if screen matches an exact string (e.g. "p01") */
  auto is(uint id) -> bool const { return get_id() == id; }

  /** True if current screen is the main screen. */
  auto is_main() -> bool const { return get_id() == 0; }

  // ------ Member variables ------
  ShellStorage& storage;
};
