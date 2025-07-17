#pragma once
#include "core/common/imports/_all.hpp"
#include "Instruction.hpp"
#include "InstructionParam.hpp"

class ProcessData; // Forward declaration

/** Represents an executable instruction handler. */
struct InstructionHandler {
  using func = function<void(const Instruction&, ProcessData&)>;
  using Schema = InstructionParam;
  using list = vector<InstructionParam::Signature>;

  // === Opcode name ===
  str opcode;           /// The opcode string representing the instruction.
  
  // === Optional Metadata ===
  str open_opcode; ///< For control instruction with a matching open
  str exit_opcode; ///< For control instruction with a matching exit
  list signatures;      ///< Describes expected argument types
  
  // === Execute function ===
  func execute;         ///< Function to execute the instruction
  
  // === Helpers ===

  /** Returns true if this instruction begins a open control block (e.g., FOR). */
  bool is_control_open() const { return !exit_opcode.empty(); }

  /** Returns true if this instruction begins a exit control block (e.g., FOR). */
  bool is_control_exit() const { return !open_opcode.empty(); }

  /** Generates a random instruction based on the signature. */
  Instruction generate() const {
    auto inst = Instruction(opcode);
    if (signatures.empty())
      return inst;

    const auto& signature = Rand::pick(signatures);
    inst.args.reserve(signature.size()); // Micro-optimization: Preallocate space

    for (const auto& rule: signature)
      inst.args.push_back(rule.generate());
    return inst;
  }
};
