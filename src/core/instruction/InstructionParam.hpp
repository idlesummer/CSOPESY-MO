#pragma once
#include "core/common/imports/_all.hpp"

namespace csopesy {

  /** Represents a schema used to validate and generate instruction arguments. */
  struct InstructionParam {
    using list = vector<str>;
    using Signature = vector<InstructionParam>;

    /** Supported argument types. */
    enum class Type {
      UInt,     ///< Unsigned integer (with optional min/max)
      String,   ///< Variable or string identifier
      Keyword,  ///< Fixed set of enum-like options
    };

    Type type;    ///< The argument's expected type
    uint min = 0; ///< Minimum value (for UInt)
    uint max = 0; ///< Maximum value (for UInt)
    list options; ///< Allowed values (for Keyword)

    // === Argument Generator ===

    /** Returns a randomly generated argument string that matches this schema. */
    str generate() const {
      switch (type) {
        case Type::UInt:    return to_string(Random::num(min, max));
        case Type::String:  return Random::pick(vars);
        case Type::Keyword: return Random::pick(options);
        default:            return "<?>";  // Should never be hit
      }
    }

    // === Static Constructors ===

    static InstructionParam UInt(uint min, uint max) { return { Type::UInt, min, max }; }
    static InstructionParam UInt8() { return UInt(0, 255); }
    static InstructionParam UInt16() { return UInt(0, 65535); }
    static InstructionParam String() { return { Type::String }; }
    static InstructionParam Keyword(list opts) { return { Type::Keyword, 0, 0, opts };}

    private:

    // Shared variable name list outside
    inline static const list vars = {"x", "msg", "val", "temp", "flag"};

  };
}
