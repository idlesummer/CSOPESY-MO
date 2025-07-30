#pragma once

#include <string>
#include <iostream>
#include <algorithm>

using std::min;
using std::max;
using std::string;

#if defined(_WIN32)
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #include <windows.h>
#else
  #include <sys/ioctl.h>
  #include <unistd.h>
  #include <locale>
  #include <cwchar>     // wcwidth
  #include <codecvt>    // std::wstring_convert
#endif

// === Unicode output ===
void initialize_terminal() {
  #if defined(_WIN32)
    // Enable ANSI escape sequences
    auto h = GetStdHandle(STD_OUTPUT_HANDLE);
    auto mode = DWORD(0);
    if (GetConsoleMode(h, &mode)) {
      mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      SetConsoleMode(h, mode);
    }

    // Enable UTF-8 output
    SetConsoleOutputCP(CP_UTF8);
  #else
    std::ios_base::sync_with_stdio(false);
    std::setlocale(LC_ALL, "");
    std::wcout.imbue(std::locale(""));
  #endif
}

// === Terminal width detection ===
auto get_terminal_width() -> uint32_t {
  #if defined(_WIN32)
    auto csbi = CONSOLE_SCREEN_BUFFER_INFO{};
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
      return csbi.srWindow.Right - csbi.srWindow.Left + 1;
  #else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
      return w.ws_col;
  #endif
    return 80; // fallback
}

// === Unicode-aware visual width of a string ===
auto len(const std::string& utf8) -> int {
  #if defined(_WIN32)
    // Convert UTF-8 → UTF-16 wide string
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    if (wlen <= 0) return 0;

    auto wstr = std::wstring(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);

    // Estimate width
    int width = 0;
    for (wchar_t wc : wstr) {
      if (wc == L'\0') break;
      if (wc < 32 || (wc >= 0x7f && wc <= 0x9f)) continue;
      width += (wc >= 0x1100 && (
        wc <= 0x115f ||                   // Hangul Jamo
        wc == 0x2329 || wc == 0x232a ||   // Angle brackets
        (wc >= 0x2e80 && wc <= 0xa4cf) || // CJK, Kana, Yi
        (wc >= 0xac00 && wc <= 0xd7a3) || // Hangul syllables
        (wc >= 0xf900 && wc <= 0xfaff) || // CJK Compatibility
        (wc >= 0xfe10 && wc <= 0xfe19) || // Vertical forms
        (wc >= 0xff01 && wc <= 0xff60) || // Fullwidth ASCII
        (wc >= 0xffe0 && wc <= 0xffe6)))  // Fullwidth symbols
        ? 2 : 1;
    }
    return width;
  #else
    // Use locale-aware wcwidth
    auto conv = std::wstring_convert<std::codecvt_utf8<wchar_t>>{};
    auto wstr = conv.from_bytes(utf8);

    int width = 0;
    for (wchar_t wc : wstr) {
      int w = wcwidth(wc);
      width += (w >= 0) ? w : 0;
    }
    return width;
  #endif
}
