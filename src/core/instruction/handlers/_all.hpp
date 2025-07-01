#pragma once
#include "core/instruction/types.hpp"
#include "add.hpp"
#include "declare.hpp"
#include "for.hpp"
#include "print.hpp"
#include "sleep.hpp"
#include "subtract.hpp"

namespace csopesy::instruction {
  using list = vector<InstructionHandler>;
  
  list get_all() {
    return {
      make_add(),
      make_declare(),
      make_for(),
      make_print(),
      make_sleep(),
      make_subtract(),
    };
  }
}
