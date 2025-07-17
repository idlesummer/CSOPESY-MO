#pragma once
#include "core/common/imports/_all.hpp"


class Ansi {

  public:

  static void enable() {
    auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
    auto mode = DWORD(0);
    
    if (!GetConsoleMode(handle, &mode))
      return void(cerr << "Failed to get console mode.\n");

    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(handle, mode)) 
      return void(cerr << "Failed to enable ANSI escape sequences.\n");
  }
  
  static void set_title(ostream& out, const str& title) {
    out << "\033]0;" << title << "\a";
  }

  static void enter_alt_buffer(ostream& out) {
    out << "\033[?1049h";
  }

  static void exit_alt_buffer(ostream& out) {
    out << "\033[?1049l";
  }

  static void hide_cursor(ostream& out) {
    out << "\033[?25l";
  }

  static void show_cursor(ostream& out) {
    out << "\033[?25h";
  }

  static void clear_screen(ostream& out) {
    out << "\033[2J\033[H";
  }

  static void move_cursor(ostream& out, int row, int col) {
    out << "\033[" << row << ";" << col << "H";
  }
};
