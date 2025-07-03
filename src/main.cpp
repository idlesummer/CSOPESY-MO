#include "core/common/imports/_all.hpp"
#include "core/common/Ansi.hpp"
#include "core/common/EventEmitter.hpp"
#include "core/shell/Shell.hpp"

int main() {
  using namespace csopesy;

  system("cls");
  Ansi::enable();

  auto running = atomic_bool(true);
  auto global  = EventEmitter();
  auto shell   = Shell(global);

  global.on("shutdown", [&] {
    cout << "[Shell] Shutting down...";
    sleep_for(200ms);
    running = false;
  });

  shell.start();

  while (running) {
    global.emit("tick");
    global.dispatch();
    sleep_for(100ms);
  }

  shell.stop();  // ✅ Only join after the Shell thread finishes
  system("cls");
}
