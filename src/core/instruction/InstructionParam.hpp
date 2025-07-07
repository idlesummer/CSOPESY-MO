#pragma once
#include "core/common/imports/_all.hpp"

namespace csopesy {

  /** Represents a schema used to validate and generate instruction arguments. */
  struct InstructionParam {
    using list = vector<str>;
    using Signature = vector<InstructionParam>;

    /** Supported argument types. */
    enum class Type {
      UInt,       ///< Unsigned integer (with optional min/max)
      Str,        ///< Variable or string identifier
      Var         ///< Fixed set of variables
    };

    Type type;    ///< The argument's expected type
    uint min = 0; ///< Minimum value (for UInt)
    uint max = 0; ///< Maximum value (for UInt)

    // === Argument Generator ===

    /** Returns a randomly generated argument string that matches this schema. */
    str generate() const {
      switch (type) {
        case Type::UInt:  return to_string(Random::num(min, max));
        case Type::Str:   return Random::pick(messages);
        case Type::Var:   return Random::pick(vars);
        default:          return "<?>";  // Should never be hit
      }
    }

    // === Static Constructors ===

    static InstructionParam Var() { return { Type::Var }; }
    static InstructionParam Str() { return { Type::Str }; }
    static InstructionParam UInt(uint min, uint max) { return { Type::UInt, min, max }; }
    static InstructionParam UInt8() { return UInt(0, 255); }
    static InstructionParam UInt16() { return UInt(0, 65535); }
    
    private:

    inline static const list vars = {"x", "y", "z"};
    inline static const list messages = {
      "We use the observer pattern with an event emitter.",
      "All components have start, stop, and tick methods.",
      "We prefer composition over inheritance to build flexible and reusable components.",
      "We use data-driven design instead of deep inheritance.",
      "Each part has a single responsibility, keeping code modular.",
      "We avoid deep nesting by using guard clauses.",
      "The code is clean and easy to read.",
      "No 'vibecoding' here — everything is intentional.",
    };
  };
}
