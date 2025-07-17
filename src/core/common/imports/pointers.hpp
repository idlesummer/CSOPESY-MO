#pragma once
#include <memory>


using std::make_unique;
using std::make_shared;

// === Aliases ===

template <typename Type>
using uptr = std::unique_ptr<Type>;

template <typename Type>
using sptr = std::shared_ptr<Type>;
