#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"
#include "core/instruction/InstructionHandler.hpp"
#include "core/process/ProcessData.hpp"


/**
 * @brief Implements the SUBTRACT instruction.
 * 
 * Syntax: `SUBTRACT x y z` → `x = max(0, y - z)`
 * - Resolves both operands (y and z) via resolve().
 * - Clamps result to 0 (no negative values for uint16).
 * - Stores the result in x via set().
 * 
 * Behavior:
 * - If y or z is undeclared, declares them with 0.
 * - Memory accesses may cause page faults or violations.
 */
auto make_subtract() -> InstructionHandler {
  return InstructionHandler()
    .set_opcode("SUBTRACT")
    .add_signature(Signature().Var().Var().Var())
    .add_signature(Signature().Var().Var().Uint16())
    .add_signature(Signature().Var().Uint16().Uint16())

    .set_execute([](Instruction& inst, ProcessData& process) {
      auto& program = process.program;
      auto& memory = process.memory;
      auto& args = inst.args;

      auto [lval, lviolation, lfault, lundeclared] = memory.resolve(args[1]);
      auto [rval, rviolation, rfault, rundeclared] = memory.resolve(args[2]);

      if (lviolation || rviolation) {
        process.log("[SUBTRACT] Violation during operand read.");
        // program.terminate();
        // return;
      }

      if (lundeclared)
        memory.set(args[1], 0u);

      if (rundeclared)
        memory.set(args[2], 0u);

      auto diff = (lval > rval) ? (lval - rval) : 0u;
      auto [is_violation, is_page_fault, is_symbol_limit] = memory.set(args[0], diff);

      if (is_violation) {
        process.log("[SUBTRACT] Violation during result write.");
        program.terminate();
        return;
      }

      if (lfault || rfault || is_page_fault) {
        process.log("[SUBTRACT] Unable to resolve page fault on first try.");
        program.set_ip(program.ip); // Retry on next tick
      }  
      else if (is_symbol_limit)
        process.log("[SUBTRACT] Symbol table full — could not DECLARE target variable.");
    });
}
