#pragma once
#include "core/command/Command.hpp"
#include "core/command/CommandHandler.hpp"
#include "core/shell/internal/Shell.impl.hpp"
#include "core/process/Process.hpp"
#include "core/execution/Core.hpp"
#include "core/memory/MemoryManagerData.hpp"


auto make_process_smi() -> CommandHandler {
  return CommandHandler()
    .set_name("process-smi")
    .set_desc("Shows the current process status, logs, and info.")
    .set_min_args(0)
    .set_max_args(0)

    .set_validate([](Command& command, Shell& shell) -> optional<str> {
      return nullopt;
    })

    .set_execute([](Command& command, Shell& shell) {
      auto& scheduler = shell.scheduler;
      auto& storage = shell.storage;
      auto& memory = shell.scheduler.data.memory;
      auto& mm_data = memory.data;
          
      uint total_frames = mm_data.frame_count;
      uint page_size    = mm_data.page_size;
      uint free_frames  = mm_data.free_frames.size();
      uint used_frames  = total_frames - free_frames;

      uint used_bytes   = used_frames * page_size;
      uint total_bytes  = total_frames * page_size;

      uint used_mib     = used_bytes / 1024;
      uint total_mib    = total_bytes / 1024;
      uint memory_util  = (used_frames * 100) / total_frames;

      if (shell.screen.is_main()) {
        // System Info
        cout << "------------------------------------------------------------\n";
        cout << "| PROCESS-SMI V01.00 Driver Version: 01.00 |\n";
        cout << "------------------------------------------------------------\n";
        cout << "CPU-Util:  100%\n";
        cout << "Memory Usage: " << used_mib << "MiB / " << total_mib << "MiB\n";
        cout << "Memory Util: " << memory_util << "%\n";


        // Process list
        cout << "\n------------------------------------------------------------\n";
        cout << "Running processes and memory usage:\n\n";

        for (auto& [pid, table] : mm_data.page_table_map) {
          uint frames = 0;
          for (auto& [_,page] : table.entries) {
            if (page.is_loaded()) {
              frames++;
            }
          }
          uint used_bytes = frames * mm_data.page_size;
          uint used_mib = used_bytes / 1024;

          cout << format("process{:02d} {}MiB\n", pid, used_mib);
        }
      } else if (!shell.screen.is_main()) {
        uint pid = shell.screen.get_id();
        auto& process = scheduler.data.get_process(pid);
        auto& program = process.data.program;

        cout << format("Process name: {}\n", process.data.name);
        cout << format("ID: {}\n", process.data.core_id);

        cout << "Logs:\n";
        for (auto& log: process.data.logs)
          cout << format("{}\n", log);

        cout << format("Current instruction line: {}\n", program.ip);
        cout << format("Lines of code: {}\n\n", program.script.size());

        shell.storage.remove("process-smi.pid");
      }
    });
}
