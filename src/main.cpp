#include "core/common/imports/_all.hpp"
#include "core/common/utility/EventEmitter.hpp"
#include "core/shell/Shell.hpp"

int main() {
  using namespace csopesy;

  auto running = abool(true); // Global run flag for main loop
  auto global  = EventEmitter();    // Central event system
  auto shell   = Shell(global);     // Main shell interface

  // Listen for shutdown event to stop main loop
  global.on("shutdown", [&] {
    cout << "[Shell] Shutting down...";
    sleep_for(200ms);
    running = false;
  });

  shell.start();  // Launch shell thread

  // === Main simulation loop ===
  while (running) {
    global.emit("tick");  // Broadcast system tick
    global.dispatch();    // Run all queued listeners
    sleep_for(100ms);     // Simulated tick rate
  }
  
  shell.stop(); // Join shell thread (cleanup)
}
