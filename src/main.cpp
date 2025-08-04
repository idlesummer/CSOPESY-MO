#include "core/common/imports/_all.hpp"
#include "core/shell/Shell.hpp"


int main() {

  auto tokens = re::split("Hello;world", ";");

  for (auto& token : tokens)
    cout << format("{}\n", token);

  // auto shell = Shell(); // Main shell interface
  // shell.start();        // Launch shell thread
}
