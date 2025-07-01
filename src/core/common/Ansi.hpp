#pragma once
#include "core/common/imports/_all.hpp"

namespace csopesy::Ansi {
  inline void enable() {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (!GetConsoleMode(handle, &mode))
      throw runtime_error("Failed to get console mode");

    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(handle, mode))
      throw runtime_error("Failed to enable ANSI virtual terminal");
  }

  inline void set_title(ostream& out, const str& title) {
    out << "\033]0;" << title << "\a";
  }

  inline void enter_alt_buffer(ostream& out) {
    out << "\033[?1049h";
  }

  inline void exit_alt_buffer(ostream& out) {
    out << "\033[?1049l";
  }

  inline void hide_cursor(ostream& out) {
    out << "\033[?25l";
  }

  inline void show_cursor(ostream& out) {
    out << "\033[?25h";
  }

  inline void clear_screen(ostream& out) {
    out << "\033[2J\033[H";
  }

  inline void move_cursor(ostream& out, int row, int col) {
    out << "\033[" << row << ";" << col << "H";
  }
}
