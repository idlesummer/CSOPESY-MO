#pragma once
#include "core/common/imports/_all.hpp"
#include "core/common/constants/banner.hpp"
#include "core/common/Component.hpp"
#include "core/common/EventEmitter.hpp"
#include "core/scheduler/Scheduler.hpp"
#include "core/command/CommandInterpreter.hpp"
#include "types.hpp"

namespace csopesy {

  class Shell: public Component {
    using Interpreter = CommandInterpreter;

    Thread thread;
    atomic_bool active = true;
    Screen current_screen = Screen::MAIN_MENU;
    Str active_process_name;
    
    /** Sub-components */
    Interpreter& interpreter;
    Scheduler scheduler; 

    public:
    
    Shell(EventEmitter& emitter);

    void start() override {
      global.on("tick", [&] { 
        scheduler.tick(); 
      });

      thread = Thread([&] {
        while (active) 
          tick();
      });
    }

    // only the main thread calls this pls
    void stop() override {
      active = false;

      if (thread.joinable()) 
        thread.join();
    }

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

    void request_stop() {
      active = false;  // safe from inside the shell thread
    } 

    void emit(str name, any data={}) {
      global.emit(move(name), move(data));
    }

    // screen stuff
    void switch_screen(Screen new_screen, Str proc_name=nullopt) {
      current_screen = new_screen;
      active_process_name = proc_name;

      switch (new_screen) {
        case Screen::MAIN_MENU:
          system("cls");
          cout << BANNER << '\n';
          active_process_name = nullopt;
          break;

        case Screen::PROCESS_VIEW:
          system("cls");
          break;

        default:
          cout << "Unknown screen.\n";
          break;
      }
    }

    Screen get_screen() const {
      return current_screen;
    }

    str screen_name(Screen s) const {
      switch (s) {
        case Screen::MAIN_MENU: return "main";
        case Screen::PROCESS_VIEW: return "process";
        default: return "unknown";
      }
    }
    
    Str get_active_process_name() {
      return active_process_name;
    }

    Scheduler& get_scheduler() {
      return scheduler;
    }
  };
}
