#pragma once
#include "core/shell/ShellStorage.hpp"
#include "core/common/constants/banner.hpp"


class ShellScreen {
  using Storage = ShellStorage;
  Storage& storage;
  
  public:
  
  /** Initializes the screen and defaults to the main screen (ID 0). */
  ShellScreen(Storage& storage): storage(storage) {
    storage.set("screen.id", uint(0));
  }

  /** Switch to a screen by ID. Use "main" or "" for the main screen. */
  void switch_to(uint id) {
    storage.set("screen.id", id);
    if (is_main()) cout << BANNER << '\n';
  }

  /** Switch to the main screen (empty ID). */
  void switch_to_main() {
    switch_to(0);
  }

  /** Returns the screen ID string. */
  uint get_id() const {
    return storage.get<uint>("screen.id");
  }

  /** Helper: Check if screen matches an exact string (e.g. "p01") */
  bool is(uint id) const {
    return get_id() == id;
  }

  /** True if current screen is the main screen. */
  bool is_main() const {
    return get_id() == 0;
  }
};
