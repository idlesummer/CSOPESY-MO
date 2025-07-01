#pragma once
#include "core/common/imports/_all.hpp"
#include "core/shell/Shell.hpp"
#include "core/command/types.hpp"
#include "core/scheduler/types.hpp"

namespace csopesy::command {
  inline const CommandHandler make_initialize() {
    using list = vector<str>;
    return {
      .name = "initialize",
      .desc = "Initializes the processor configuration of the application.",
      .min_args = 0,
      .max_args = 0,
      .flags = {},

      .validate = [](const Command& command, Shell& shell) -> Str {
        // Check if the scheduler has already been initialized
        if (shell.get_scheduler().get_config().initialized)
          return Str{"[Shell] Already initialized."};
        
        // Check if the file could not be opened or is empty
        auto lines = read_lines("config.txt");
        if (lines.empty())
          return Str{"[Shell] Failed to open config.txt"};

        command.context = any(move(lines));
        return nullopt;
      },

      .execute = [](const Command& command, Shell& shell) {
        const auto& lines = command.get_context<list>();
        SchedulerConfig config;  // Start fresh config (don't modify existing directly)

        for (const auto& line : lines) {
          str key, value;
          isstream(line) >> key >> value;

          if (config.set(key, move(value))) continue;
          cout << format("[Shell] Unknown config key: {}\n", key);
        }

        config.initialized = true;
        shell.get_scheduler().set_config(config);  // ✅ Apply to scheduler

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
        
        cout << "[Shell] Scheduler config loaded.";
      },
    };
  }
}
