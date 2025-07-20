#pragma once
#include "core/common/imports/_all.hpp"
#include "Instruction.hpp"
#include "InstructionParam.hpp"

class ProcessData; // Forward declaration

/** Represents an executable instruction handler. */
class InstructionHandler {
  public:

  InstructionHandler():
    opcode      (""s),                                // The opcode string representing the instruction.
    open_opcode (""s),                                // For control instruction with a matching open
    exit_opcode (""s),                                // For control instruction with a matching exit
    signatures  (vec<InstructionParam::Signature>()), // Describes expected argument types
    execute     (nullptr) {}                          // Function to execute the instruction
    
  // Chainable setters
  auto set_opcode(str op) -> InstructionHandler { return opcode = move(op), *this; }
  auto set_open_opcode(str op) -> InstructionHandler { return open_opcode = move(op), *this; }
  auto set_exit_opcode(str op) -> InstructionHandler { return exit_opcode = move(op), *this; }
  auto add_signature(vec<InstructionParam> sig) -> InstructionHandler {
    return signatures.push_back(std::move(sig)), *this;
  }
  auto set_execute(func<void(Instruction&,ProcessData&)> fn) -> InstructionHandler {
    return execute = move(fn), *this;
  }

  /** Returns true if this instruction begins a open control block (e.g., FOR). */
  auto is_control_open() const -> bool { return !exit_opcode.empty(); }

  /** Returns true if this instruction begins a exit control block (e.g., FOR). */
  auto is_control_exit() const -> bool { return !open_opcode.empty(); }

  /** Generates a random instruction based on the signature. */
  auto generate() const -> Instruction {
    auto inst = Instruction(opcode);
    if (signatures.empty())
      return inst;

    const auto& signature = Rand::pick(signatures);
    inst.args.reserve(signature.size()); // Micro-optimization: Preallocate space

    for (const auto& rule: signature)
      inst.args.push_back(rule.generate());
    return inst;
  }

  // ------ Member variables ------

  str opcode;                                     
  str open_opcode;                                
  str exit_opcode;                                
  vec<InstructionParam::Signature> signatures;    
  func<void(Instruction&,ProcessData&)> execute;  
};
