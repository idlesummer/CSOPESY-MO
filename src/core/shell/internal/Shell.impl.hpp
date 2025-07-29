#pragma once
#include "core/common/imports/_all.hpp"
#include "core/common/utility/Ansi.hpp"
#include "core/common/utility/Text.hpp"
#include "core/command/CommandInterpreter.hpp"
#include "core/scheduler/Scheduler.hpp"

// Shell-specific includes
#include "core/shell/ShellBanner.hpp"
#include "core/shell/ShellStorage.hpp"
#include "core/shell/ShellScreen.hpp"


/** @brief Interactive shell managing CLI, events, and system ticks. */
class Shell {  
  public:

  Shell(): 
    interpreter   (CommandInterpreter::get()),  // Instance of the command interpreter
    shell_thread  (),                           // Shell thread (starts later in start())
    system_thread (),                           // Shell thread (starts later in start())
    shell_active  (atomic_bool{true}),          // Atomic flag to control shell lifecycle
    system_active (atomic_bool{true}),          // Atomic flag to control shell lifecycle
    scheduler     (Scheduler()),                // Owned scheduler instance
    banner        (ShellBanner()),              // For printing the shell banner
    storage       (ShellStorage()),             // Storage for dynamic shell/session data
    screen        (storage) {}                  // Owned instance of shell screen manager

  /** @brief Starts shell and system threads and blocks until shutdown. */
  void start() {
    initialize();
    system("cls");
    cout << '\n'; // Leave line 1 blank

    // Start shell (input) and system (scheduler) threads
    shell_thread  = Thread([&] { 
      while (shell_active)  
        tick_shell(); 
    });

    system_thread = Thread([&] { 
      while (system_active) 
        tick_system(); 
    });

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

  // ------ Instance variables ------

  CommandInterpreter& interpreter;    
  Thread shell_thread;
  Thread system_thread;
  atomic_bool shell_active;
  atomic_bool system_active;
  Scheduler scheduler; 
  ShellBanner banner;
  ShellStorage storage;
  ShellScreen screen;

  // ------ Internal helpers ------
  private:

  /** @brief Populates the CommandInterpreter with all available commands (See impl in Shell.hpp). */
  void register_commands();

  /** @brief Initializes terminal and registers all event listeners. */
  void initialize() {
    Text::enable();       // Prepare terminal settings for output
    register_commands();  // Initialize command handlers
  }

  /** @brief Executes one shell input cycle. Called repeatedly by shell thread. */
  void tick_shell() {
    cout << ">>> " << flush;
    if (str input; getline(cin, input)) {
      with_locked([&]{
        interpreter.execute(move(input), *this);  // Executes commands
        cout << '\n';
      });

    } else {
      cout << "[Shell] Input stream closed.\n" << flush;
      stop(true);
    }
  }

  /** @brief Executes one scheduler tick. Called repeatedly by system thread. */
  void tick_system() {
    with_locked([&] {
      scheduler.tick();
    });

    sleep_for(100ms);   // Tick interval
  }
};
