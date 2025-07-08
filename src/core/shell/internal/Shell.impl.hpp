#pragma once
#include "core/common/imports/_all.hpp"
#include "core/common/utility/Ansi.hpp"
#include "core/common/constants/banner.hpp"
#include "core/common/utility/Component.hpp"
#include "core/common/utility/EventEmitter.hpp"
#include "core/scheduler/Scheduler.hpp"
#include "core/command/CommandInterpreter.hpp"

// Shell-specific includes
#include "core/shell/ShellStorage.hpp"
#include "core/shell/ShellScreen.hpp"

namespace csopesy {

  class Shell: public Component {
    using Interpreter = CommandInterpreter;
    using Storage = ShellStorage;
    using Screen = ShellScreen;
    
    // === Core system components ===
    Interpreter& interpreter;
    Scheduler scheduler; 
    
    // === Shell subcomponents ===
    Storage storage;  ///< Dynamic storage for shell data
    Screen screen;
    
    // === Control & lifecycle ===
    Thread thread;
    abool active = true;
    
    public:

    /** See Shell.hpp for implementation. */
    Shell(EventEmitter& emitter); // 

    /** Starts the shell loop in a separate thread and hooks into global ticks. */
    void start() override {
      system("cls");
      Ansi::enable();

      global.on("tick", [&] { 
        scheduler.tick(); 
      });

      thread = Thread([&] {
        cout << '\n';   // Lets make it a convention that line 1 is empty.
        while (active) 
          tick();
      });
    }

    /** Stops the shell and joins the thread. Safe to call multiple times. */
    void stop() override {
      active = false;

      if (thread.joinable()) 
        thread.join();

      system("cls");
    }

    /** Executes a single shell tick (input + command dispatch). */
    void tick() override {
      cout << ">>> " << flush;

      if (str input; getline(cin, input)) {
        interpreter.execute(move(input), *this);
        cout << '\n';

      } else {
        cout << "[Shell] Input stream closed.\n";
        active = false;
      }
    }

    /** Signals the shell to stop from within the shell thread. */
    void request_stop() {
      active = false;  // safe from inside the shell thread
    } 

    /** Emits an event using the global EventEmitter. */
    void emit(str name, any data={}) {
      global.emit(move(name), move(data));
    }

    /** Returns the command interpreter instance. */
    Interpreter& get_interpreter() {
      return interpreter;
    }

    /** Returns the scheduler instance. */
    Scheduler& get_scheduler() {
      return scheduler;
    }

    /** Returns the screen controller. */
    Screen& get_screen() {
      return screen;
    }

    /** Returns the shell storage. */
    Storage& get_storage() {
      return storage;
    }
  };
}
