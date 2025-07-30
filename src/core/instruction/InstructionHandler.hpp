#pragma once
#include "core/common/imports/_all.hpp"
#include "Instruction.hpp"
#include "types.hpp"

class ProcessData; // Forward declaration

/** Represents an executable instruction handler. */
class InstructionHandler {
  public:

  InstructionHandler():
    opcode      (""s),              // The opcode string representing the instruction.
    open_opcode (""s),              // For control instruction with a matching open
    exit_opcode (""s),              // For control instruction with a matching exit
    signatures  (vec<Signature>()), // Describes expected argument types
    execute     (nullptr) {}        // Function to execute the instruction
    
  // Chainable setters
  
  /** @brief Sets the opcode for this instruction. */
  auto set_opcode(str op) -> InstructionHandler { return opcode = move(op), *this; }

  /** @brief Sets the matching open opcode (used by control exit instructions). */
  auto set_open(str op) -> InstructionHandler { return open_opcode = move(op), *this; }
  
  /** @brief Sets the matching exit opcode (used by control open instructions). */
  auto set_exit(str op) -> InstructionHandler { return exit_opcode = move(op), *this; }
  
  /** @brief Adds a signature definition to the instruction. */
  auto add_signature(Signature sig) -> InstructionHandler { return signatures.push_back(move(sig)), *this; }
  
  /** @brief Sets the execution function for this instruction. */
  auto set_execute(func<void(Instruction&,ProcessData&)> fn) -> InstructionHandler { return execute = move(fn), *this; }

  /** @brief Returns true if this instruction begins a open control block (e.g., FOR). */
  auto is_control_open() -> bool { return !exit_opcode.empty(); }

  /** @brief Returns true if this instruction begins a exit control block (e.g., FOR). */
  auto is_control_exit() -> bool { return !open_opcode.empty(); }

  /** Generates a random instruction based on the signature. */
  auto generate() -> Instruction {
    auto inst = Instruction(opcode);
    if (signatures.empty()) return inst;

    auto& sig = Rand::pick(signatures);
    inst.args = sig.generate();
    return inst;
  }

  // ------ Instance variables ------

  str opcode;                                     
  str open_opcode;                                
  str exit_opcode;                                
  vec<Signature> signatures;    
  func<void(Instruction&,ProcessData&)> execute;  
};
