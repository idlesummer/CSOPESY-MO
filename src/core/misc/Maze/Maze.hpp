#pragma once
#include "core/common/imports/_all.hpp"
#include "Position.hpp"

namespace csopesy {
  class Maze {
    using list    = vector<int>;
    using Symbols = array<str, 17>;
    using Row     = vector<byte>;
    using Grid    = vector<Row>;

    static constexpr byte PATH = 0;
    static constexpr byte STEP = 16;
    static constexpr byte WALL = 255;
    inline static const Symbols symbols = {
      " ", "║", "═", "╔", "═", "╗", "═", "╦",
      "║", "║", "╚", "╠", "╝", "╣", "╩", "╬",
      "·",
    };

    uint width;
    uint height;
    Grid grid;

    public:
    Maze(uint width, uint height): 
      width(width), 
      height(height), 
      grid(height, Row(width, WALL)) {
      generate();
    }

    str render() const {
      auto stream = osstream();
      for (const auto& row: grid) {
        for (const auto& cell: row)
          stream << symbols[cell];
        stream << '\n';
      }
      return stream.str();
    }

    private:
    bool in_bounds(int x, int y) const {
      return x >= 0 && x < width && y >= 0 && y < height;
    }

    bool is_wall(int x, int y) const {
      return grid[y][x] != PATH;
    }

    void generate() {
      using Position::pos;

      list dirs  = { pos(1,0), pos(0,1), pos(-1,0), pos(0,-1) };
      list stack = { pos(1,1) };
      stack.reserve((width * height) / 2);
      
      while (!stack.empty()) {
        // Pop the current cell (top of the stack)
        auto [x, y] = pos(stack.back());
        stack.pop_back();
        
        for (auto dir: Rand::shuffle(dirs)) {
          auto [dx, dy] = pos(dir);
          int nx = x + dx*2;
          int ny = y + dy*2;

          // Choose a random unvisited neighbor
          if (!in_bounds(nx, ny) || !is_wall(nx, ny)) 
            continue;

          // Remove the wall between the current cell and the chosen neighbor
          grid[y+dy][x+dx] = PATH;
          grid[ny][nx] = PATH;

          // Add the current position and the new position to the stack
          stack.push_back(pos(x, y));
          stack.push_back(pos(nx, ny));
          break;
        }
      }

      format();
    }

    void format() {
      for (int y=0; y < height; ++y) {
        for (int x=0; x < width; ++x) {
          // Skip empty spaces
          if (!is_wall(x, y)) continue;

          byte bits = 0;
          if (y > 0 && is_wall(x, y-1))        bits |= 0b1000;  // Check if there's a top wall
          if (x > 0 && is_wall(x-1, y))        bits |= 0b0100;  // Check if there's a left wall
          if (x < width-1 && is_wall(x+1, y))  bits |= 0b0010;  // Check if there's a right wall
          if (y < height-1 && is_wall(x, y+1)) bits |= 0b0001;  // Check if there's a bottom wall

          // Set the grid's value to the correct symbol based on the bitmask
          grid[y][x] = bits;
        }
      }
    }
  };
}
