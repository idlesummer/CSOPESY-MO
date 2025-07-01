#pragma once
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

namespace csopesy {
  
  // I/O utility aliases
  using std::cin;
  using std::cout;
  using std::cerr;
  using std::endl;
  using std::flush;
  using std::getline;
  using std::ios;
  using std::ifstream;
  using std::ofstream;
  using std::ostream;
  using std::runtime_error;

  // === Utility functions ===
  inline std::vector<std::string> read_lines(const std::string& path) {
    auto file = std::ifstream(path);
    if (!file) return {};

    auto buffer = std::ostringstream();
    buffer << file.rdbuf();

    auto in = std::istringstream(buffer.str());
    auto lines = std::vector<std::string>();
    lines.reserve(8);

    for (std::string line; std::getline(in, line); )
      lines.emplace_back(std::move(line));

    return lines;
  }
}
