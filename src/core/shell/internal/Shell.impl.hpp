#pragma once
#include "core/common/imports/_all.hpp"
#include "core/common/utility/Ansi.hpp"
#include "core/common/constants/banner.hpp"
#include "core/common/utility/EventEmitter.hpp"
#include "core/command/CommandInterpreter.hpp"
#include "core/scheduler/Scheduler.hpp"

// Shell-specific includes
#include "core/shell/ShellStorage.hpp"
#include "core/shell/ShellScreen.hpp"


class Shell {
  using Storage = ShellStorage;
  using Screen = ShellScreen;
  
  public:

  // === Core system components ===
  EventEmitter& global;
  CommandInterpreter& interpreter;
  
  // === Shell subcomponents ===
  Scheduler scheduler; 
  Storage storage;  ///< Dynamic storage for shell data
  Screen screen;

  // === Control & lifecycle ===
  Thread thread;
  abool active = true;

  // === Methods ===

  /** @brief See Shell.hpp for implementation. */
  Shell(EventEmitter& emitter);

  /** @brief Starts the shell loop in a separate thread and hooks into global ticks. */
  void start() {
    // system("cls");
    Ansi::enable();
    enable_unicode();

    // Starts the tick handler that runs the scheduler.
    global.on("tick", [&] { 
      with_locked([&] { scheduler.tick(); }); // Makes scheduler.tick() does not conflict with shell commands
    });
  
    // Start CLI in a separate thread
    thread = Thread([&] {
      cout << '\n';   // Lets make it a convention that line 1 is empty.
      while (active) 
        tick();
    });
  }

  /** @brief Stops the shell and joins the thread. Safe to call multiple times. */
  void stop() {
    active = false;
    if (thread.joinable()) 
      thread.join();

    system("cls");
  }

  /** @brief Executes a single shell tick (input + command dispatch). */
  void tick() {
    cout << ">>> " << flush;

    if (str input; getline(cin, input)) {
      with_locked([&]{
        interpreter.execute(move(input), *this);
        cout << '\n';
      });

    } else {
      cout << "[Shell] Input stream closed.\n";
      active = false;
    }
  }

  /** @brief Signals the shell to stop from within the shell thread. */
  void set_active(bool flag) { active = flag; }

  /** @brief Emits an event using the global EventEmitter. */
  void emit(str name, any data={}) { global.emit(move(name), move(data)); }
};
