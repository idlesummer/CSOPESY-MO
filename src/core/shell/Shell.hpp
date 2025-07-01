#pragma once
#include "core/common/imports/_all.hpp"
#include "core/common/Component.hpp"
#include "core/common/EventEmitter.hpp"
#include "core/scheduler/Scheduler.hpp"
#include "core/command/CommandInterpreter.hpp"
#include "core/command/types.hpp"

namespace csopesy {
  enum class Screen {
    MAIN_MENU,
    PROCESS_VIEW,
  };

  void show_header() {
    cout << R"(
           .dP                                                                        Yb    
         .dP    .ooooo.   .oooo.o  .ooooo.  oo.ooooo.   .ooooo.   .oooo.o oooo    ooo  `Yb  
        dP     d88' `"Y8 d88(  "8 d88' `88b  888' `88b d88' `88b d88(  "8  `88.  .8'     `Yb
        Yb     888       `"Y88b.  888   888  888   888 888ooo888 `"Y88b.    `88..8'      .dP
         `Yb   888   .o8 o.  )88b 888   888  888   888 888    .o o.  )88b    `888'     .dP  
           `Yb `Y8bod8P' 8""888P' `Y8bod8P'  888bod8P' `Y8bod8P' 8""888P'     .8'     dP    
                                             888                          .o..P'            
                                            o888o                         `Y8P'             

                            Like a Real OS, But Not Written in Assembly!                    
                                Dicayanan|Maramag|Maunahan|Villaver                         
    )" << endl;
  }

  class Shell: public Component {
    Thread             cli_thread;
    Thread             tick_thread;
    atomic_bool        active = true;
    CommandInterpreter interpreter;
    Scheduler          scheduler; 
    Screen current_screen = Screen::MAIN_MENU;
    std::optional<std::string> active_process_name;
    public:
    Shell(EventEmitter& emitter): Component(emitter) {}

    void start() override {
      global.on("tick", [&] { scheduler.tick(); });

      cli_thread = Thread([&] {
        while (active) tick();
      });

      // tick_thread = Thread([&] {
      //   while (active)
      //     scheduler.tick();
      // });
    }

    // only the main cli_thread calls this pls
    void stop() override {
      active = false;

      if (cli_thread.joinable()) 
        cli_thread.join();

      if (tick_thread.joinable()) 
        tick_thread.join();
    }

    void tick() override {
      str input;
      cout << ">>> " << flush;
      if (!getline(cin, input)) {
        cout << "[Shell] Input stream closed.\n";
        active = false;
        return;
      }

      interpreter.execute(move(input), *this);
      cout << '\n';
    }

    void request_stop() {
      active = false;  // safe from inside the shell cli_thread
    }

    void emit(str name, any data={}) {
      global.emit(move(name), move(data));
    }

    void register_command(CommandHandler command) {
      interpreter.register_command(move(command));
    }

    // screen stuff
    void switch_screen(Screen new_screen, std::optional<std::string> proc_name = std::nullopt) {
      current_screen = new_screen;
      active_process_name = proc_name;

      switch (new_screen) {
        case Screen::MAIN_MENU:
          system("cls");
          show_header();
          active_process_name = std::nullopt;
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

    string screen_name(Screen s) const {
      switch (s) {
        case Screen::MAIN_MENU: return "main";
        case Screen::PROCESS_VIEW: return "process";
        default: return "unknown";
      }
    }
    
    std::optional<std::string> get_active_process_name() {
      return active_process_name;
    }

    Scheduler& get_scheduler() {
      return scheduler;
    }
  };
}
