#include "core/common/imports/_all.hpp"
#include "core/shell/Shell.hpp"


int main() { 
  auto shell = Shell(); // Main shell interface
  shell.start();        // Launch shell thread
}
