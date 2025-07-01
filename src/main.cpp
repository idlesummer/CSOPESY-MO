#include "core/common/imports/_all.hpp"
#include "core/common/EventEmitter.hpp"
#include "core/shell/Shell.hpp"

// Command files
#include "core/command/handlers/_all.hpp"

void enable_ansi() {
  auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
  auto mode = DWORD(0);

  if (!GetConsoleMode(handle, &mode)) {
    std::cerr << "Failed to get console mode.\n";
    return;
  }

  mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(handle, mode)) 
    std::cerr << "Failed to enable ANSI escape sequences.\n";
}

int main() {
  using namespace csopesy;

  system("cls");
  enable_ansi();
  auto running = atomic_bool(true);
  auto global  = EventEmitter();
  auto shell   = Shell(global);

  for (auto cmd: command::get_all())
    shell.register_command(move(cmd));

  global.on("shutdown", [&] {
    cout << "[Shell] Shutting down..." << endl;
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
