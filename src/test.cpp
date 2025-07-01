#include "core/misc/Maze/Maze.hpp"


int main() {
  using namespace csopesy;
  system("chcp 65001 >nul");  // enable utf printing

  auto maze = Maze(101, 25);  // Create a 21x21 maze
  cout << maze.render();      // Print the maze to the console

  return 0;
}
