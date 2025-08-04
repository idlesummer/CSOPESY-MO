#pragma once
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"

/**
 * @brief Implements the ADD instruction.
 * 
 * Syntax: `ADD x y z` → `x = y + z`
 * - Resolves both operands (y and z) via resolve().
 * - Adds the two 16-bit values.
 * - Stores the result in x via set().
 * 
 * Behavior:
 * - If x is undeclared, attempts to declare it (if space is available).
 * - All memory accesses may cause page faults or violations.
 */
auto make_add() -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("ADD")
    .add_signature(Signature().Var().Var().Var())
    .add_signature(Signature().Var().Var().Uint16())
    .add_signature(Signature().Var().Uint16().Uint16())
    
    .set_execute([](Instruction& inst, ProcessData& process) {
      auto& program = process.program;
      auto& memory = process.memory;
      auto& args = inst.args;

      auto [lval, lviolation, lfault, lundeclared] = memory.resolve(args[1]);
      auto [rval, rviolation, rfault, rundeclared] = memory.resolve(args[2]);

      // "Variables are automatically declared with a value of 0 
      // if they have not yet been declared beforehand."
      if (lundeclared)
        memory.set(args[1], 0u);

      if (rundeclared)
        memory.set(args[2], 0u);

      auto sum = lval + rval;
      auto [is_violation, is_page_fault, is_symbol_limit] = memory.set(args[0], sum);

      // Optional logs for debugging or trace output
      if (lfault || rfault || is_page_fault) {
        process.log("[ADD] Unable to resolve page fault on first try.");
        program.set_ip(program.ip);
      }
      else if (lviolation || rviolation || is_violation)
        process.log("[ADD] Violation during operand or result write.");
      else if (is_symbol_limit)
        process.log("[ADD] Symbol table full — could not DECLARE target variable.");
    });
}
