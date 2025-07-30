#pragma once
#include <array>
#include <list>
#include <map>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <set>
#include <utility>
#include <vector>


using std::array;
using std::list;
using std::make_pair;
using std::out_of_range;
using std::pair;
using std::queue;
using std::set;
using std::tuple;
using std::unordered_map;
using std::vector;

// === Aliases ===
template <typename Type>
using vec = vector<Type>;

template <typename Key, typename Val>
using ordered_map = std::map<Key, Val>;

template <typename Key, typename Val>
using map = unordered_map<Key, Val>;
