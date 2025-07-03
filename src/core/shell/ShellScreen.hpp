#pragma once
#include "core/shell/ShellStorage.hpp"
#include "core/common/constants/banner.hpp"

namespace csopesy {

  class ShellScreen {
    using Storage = ShellStorage;
    Storage& storage;
    
    public:

    ShellScreen(Storage& storage): storage(storage) {
      storage.set("screen.id", str());
    }

    /** Switch to a screen by ID. Use "main" or "" for the main screen. */
    void switch_to(const str& id) {
      storage.set("screen.id", id);
      // system("cls"); 

      if (is_main())
        cout << BANNER << '\n';
    }

    /** Switch to the main screen (empty ID). */
    void switch_to_main() {
      switch_to("");
    }

    /** Returns the screen ID string. */
    str get_id() const {
      return storage.get<str>("screen.id");
    }

    /** Helper: Check if screen matches an exact string (e.g. "p01") */
    bool is(const str& id) const {
      return get_id() == id;
    }

    /** True if current screen is the main screen. */
    bool is_main() const {
      return get_id().empty();
    }
  };
}
