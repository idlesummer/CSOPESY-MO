#pragma once
#include "core/common/imports/_all.hpp"
#include "Instruction.hpp"
#include "InstructionSchema.hpp"

namespace csopesy {
  class ProcessData; // Forward declaration

  /** Represents an executable instruction handler. */
  struct InstructionHandler {
    using func = function<void(const Instruction&, ProcessData&)>;
    using Schema = InstructionSchema;
    using list = vector<Schema>;

    // === Opcode name ===
    str opcode;           /// The opcode string representing the instruction.
    
    // === Optional Metadata ===
    str open_opcode = ""; ///< For control instruction with a matching end
    str next_opcode = ""; ///< For control instruction with a matching end
    str exit_opcode = ""; ///< For control instruction with a matching end
    list schema;          ///< Describes expected argument types
    
    // === Execute function ===
    func execute;         ///< Function to execute the instruction
    
    // === Helpers ===

    /** Returns true if this instruction begins a control block (e.g., FOR). */
    bool is_control() const { 
      return !open_opcode.empty() || !next_opcode.empty() || !exit_opcode.empty();
    }

    /** Generates a random instruction based on the schema. */
    Instruction generate() const {
      auto inst = Instruction(opcode);
      inst.args.reserve(schema.size()); // Micro-optimization: Preallocate space

      for (const auto& rule: schema)
        inst.args.push_back(rule.generate_arg());
      return inst;
    }
  };
}
