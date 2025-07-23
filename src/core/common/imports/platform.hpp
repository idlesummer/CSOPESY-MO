#pragma once

#if defined(_WIN32)
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #include <windows.h>
  void enable_unicode() {
    SetConsoleOutputCP(CP_UTF8);
  }
#else
  #include <locale>
  #include <iostream>
  void enable_unicode() {
    std::ios_base::sync_with_stdio(false);
    std::setlocale(LC_ALL, "");
    std::wcout.imbue(std::locale(""));
  }
#endif

#include <algorithm>
using std::min;
using std::max;
