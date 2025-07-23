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


/** @brief Interactive shell managing CLI, events, and system ticks. */
class Shell {  
  public:

  Shell(): 
    global        (EventEmitter()),             // Reference to the global EventEmitter
    interpreter   (CommandInterpreter::get()),  // Instance of the command interpreter
    shell_thread  (),                           // Shell thread (starts later in start())
    system_thread (),                           // Shell thread (starts later in start())
    shell_active  (atomic_bool{true}),          // Atomic flag to control shell lifecycle
    system_active (atomic_bool{true}),          // Atomic flag to control shell lifecycle
    scheduler     (Scheduler()),                // Owned scheduler instance
    storage       (ShellStorage()),             // Storage for dynamic shell/session data
    screen        (storage) {}                  // Owned instance of shell screen manager

  /** @brief Starts shell and system threads and blocks until shutdown. */
  void start() {
    initialize();
    system("cls");
    cout << '\n'; // Leave line 1 blank

    shell_thread  = Thread([&] { while (shell_active)  run_shell_tick(); });
    system_thread = Thread([&] { while (system_active) run_system_tick(); });

    // Wait until both threads finish (safe from main thread)
    if (shell_thread.joinable())  shell_thread.join();
    if (system_thread.joinable()) system_thread.join();
    system("cls");
  }

  /** @brief Triggers shutdown. Must be called inside shell thread only. */
  void stop(bool silent=false) {
    if (!shell_active && !system_active)
      return;

    if (!silent)
      cout << "[Shell] Shutting down...\n" << flush;

    shell_active = false;
    system_active = false;
    sleep_for(300ms);       // Optional pause for effect
  }

  /** @brief Emits a named event with optional payload. */
  void emit(str name, any data={}) { 
    global.emit(move(name), move(data)); 
  }

  // ------ Member variables ------

  EventEmitter global;
  CommandInterpreter& interpreter;    
  Thread shell_thread;
  Thread system_thread;
  atomic_bool shell_active;
  atomic_bool system_active;
  Scheduler scheduler; 
  ShellStorage storage;
  ShellScreen screen;

  // ------ Internal helpers ------
  private:

  /** @brief Populates the CommandInterpreter with all available commands (See impl in Shell.hpp). */
  void register_commands();

  /** @brief Initializes terminal and registers all event listeners. */
  void initialize() {

    // Initialize terminal settings
    Ansi::enable();
    enable_unicode();
    
    // Initialize command handlers
    register_commands();
    
    // Register commands; System ticks go to scheduler
    global.on("tick", [&] {
      with_locked([&] {
        scheduler.tick();
      });
    });
  }

  /** @brief Runs the interactive shell input loop. Reads input and executes commands*/
  void run_shell_tick() {
    cout << ">>> " << flush;
    if (str input; getline(cin, input)) {
      with_locked([&]{
        interpreter.execute(move(input), *this);
        cout << '\n';
      });

    } else {
      cout << "[Shell] Input stream closed.\n" << flush;
      stop(true);
    }
  }

  /** @brief Runs the system tick loop dispatching "tick" events. */
  void run_system_tick() {
    while (system_active) {
      global.emit("tick");
      global.dispatch();
      sleep_for(100ms);   // Tick interval
    }
  }
};
