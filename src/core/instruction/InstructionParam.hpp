#pragma once
#include "core/common/imports/_all.hpp"


/** Represents a schema used to validate and generate instruction arguments. */
struct InstructionParam {
  using list = vector<str>;
  using Signature = vector<InstructionParam>;

  /** Supported argument types. */
  enum class Type {
    UInt,       // Unsigned integer (with optional min/max)
    Str,        // Variable or string identifier
    Var         // Fixed set of variables
  };

  Type type;    // The argument's expected type
  uint min = 0; // Minimum value (for UInt)
  uint max = 0; // Maximum value (for UInt)

  /** Returns a randomly generated argument string that matches this schema. */
  auto generate() -> str {
    switch (type) {
      case Type::UInt:  return to_string(Rand::num(min, max));
      case Type::Str:   return Rand::pick(messages);
      case Type::Var:   return Rand::pick(vars);
      default:          return "<?>";  // Should never be hit
    }
  }

  // === Static Constructors ===

  static auto Var() -> InstructionParam { return { Type::Var }; }
  static auto Str() -> InstructionParam { return { Type::Str }; }
  static auto UInt(uint min, uint max) -> InstructionParam { return { Type::UInt, min, max }; }
  static auto UInt8() -> InstructionParam { return UInt(0, 255); }
  static auto UInt16() -> InstructionParam { return UInt(0, 65535); }
  
  private:

  inline static const list vars = {"x", "y", "z"};
  inline static const list messages = {
    "Lorem", "ipsum", "dolor", "sit", "amet", "consectetur", "adipiscing", "elit",
  };
};
