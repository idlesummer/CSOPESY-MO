#pragma once
#include <array>
#include <list>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>


using std::array;
using std::list;
using std::make_pair;
using std::out_of_range;
using std::pair;
using std::queue;
using std::unordered_map;
using std::unordered_set;
using std::vector;

// === Aliases ===
template <typename Type>
using vec = vector<Type>;

template <typename Type>
using set = unordered_set<Type>;

template <typename Key, typename Val>
using ordered_map = std::map<Key, Val>;

template <typename Key, typename Val>
using map = unordered_map<Key, Val>;
