#pragma once 
#include "core/common/imports/_all.hpp"


/** Contains the flag name and whether it requires an argument. */
struct CommandFlag {
  str  name;              ///< Flag name (e.g., "-s", "-r", "-ls")
  bool has_arg = false;   ///< Whether the flag requires an argument
};
