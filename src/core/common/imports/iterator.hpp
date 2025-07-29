#pragma once
#include <iterator>   // for std::next
#include <ranges>     // for ranges algorithms
#include <algorithm>  // for std::replace, std::erase_if

// Iterator utilities
using std::next;

// Range-based algorithms
using std::ranges::all_of;
using std::ranges::any_of;
using std::ranges::count_if;
using std::ranges::find_if;
using std::ranges::none_of;
using std::ranges::replace_if;

// STL container utility
using std::erase_if;
