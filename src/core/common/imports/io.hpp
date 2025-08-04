#pragma once
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

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


/** @brief Reads all lines from a text file into a vector of strings. */
auto read_lines(const std::string& path) -> std::vector<std::string> {
  auto file = std::ifstream(path);
  if (!file) return {};

  auto buffer = std::ostringstream();
  buffer << file.rdbuf();

  auto in = std::istringstream(buffer.str());
  auto lines = std::vector<std::string>();

  for (std::string line; std::getline(in, line); )
    lines.emplace_back(std::move(line));

  return lines;
}
