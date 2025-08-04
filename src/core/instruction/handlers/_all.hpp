#pragma once
#include "core/instruction/InstructionHandler.hpp"
// #include "add.hpp"
#include "declare.hpp"
#include "endfor.hpp"
#include "for.hpp"
#include "print.hpp"
#include "sleep.hpp"
// #include "subtract.hpp"


auto get_instruction_handlers() -> vec<InstructionHandler> {
  return {
    // make_add(),
    make_declare(),
    make_endfor(),
    make_for(),
    make_print(),
    make_sleep(),
    // make_subtract(),
  };
}
