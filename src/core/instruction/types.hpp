#pragma once
#include "core/common/imports/_all.hpp"


/** @brief Represents one argument in an instruction signature. */
class Arg {
  public:

  Arg(str type, uint min=0, uint max=0): 
    type (move(type)),  // Parameter type: "str", "uint", or "var"
    min  (min),         // Minimum value (for "uint" type)
    max  (max) {}       // Maximum value (for "uint" type)
  
  // ------ Instance variables ------
  str type; 
  uint min; 
  uint max; 
};

/** @brief Builder for defining and generating argument signatures for instructions. */
class Signature {
  public:

  Signature(): 
    args(vec<Arg>()) {}   // Initializes an empty signature

  // === Builders ===

  /** @brief Adds a string argument. */
  auto Str() -> Signature& { return add("str"); }

  /** @brief Adds a variable name argument. */
  auto Var() -> Signature& { return add("var"); }

  /** @brief Adds an unsigned integer argument. */
  auto Uint(uint min, uint max) -> Signature& { return add("uint", min, max); }

  /** @brief Adds an unsigned 8-bit integer argument (0–255). */
  auto Uint8() -> Signature& { return Uint(0, 255); }

  /** @brief Adds an unsigned 16-bit integer argument (0–65535). */
  auto Uint16() -> Signature& { return Uint(0, 65535); }

  // === Accessors ===

  /** @brief Returns the full list of argument specs. */
  auto get() -> vec<Arg>& { return args; }

  /** @brief Returns the number of arguments in the signature. */
  auto size() -> uint { return args.size(); }

  // === Generator (optional utility) ===

  /** @brief Generates a list of example argument values matching the signature. */
  auto generate() -> vector<str> {
    auto result = vector<str>();
    for (auto& arg: args) {
      if (arg.type == "uint")     result.push_back(to_string(Rand::num(arg.min, arg.max)));
      else if (arg.type == "str") result.push_back(Rand::pick(messages));
      else if (arg.type == "var") result.push_back(Rand::pick(vars));
      else                        result.push_back("<?>");
    }
    return result;
  }

  /** @brief Returns true if the given argument list matches this signature. */
  auto matches(const vec<str>& inputs) -> bool {
    if (inputs.size() != args.size())
      return false;

    for (auto i = 0u; i < args.size(); ++i) {
      auto& expected = args[i];
      auto& actual = inputs[i];

      if (expected.type == "uint") {
        auto value =  stoui(actual);
        if (value < expected.min || value > expected.max)
          return false;

      } else if (expected.type == "str") {
        if (actual.empty()) return false;  // or remove if empty is okay

      } else if (expected.type == "var") {
        if (actual.empty() || !isalpha(actual[0])) return false;
        for (char c : actual)
          if (!isalnum(c) && c != '_') return false;

      } else {
        return false; // unknown type
      }
    }
    return true;
  }

  private:

  // ------ Instance variables ------
  vec<Arg> args;

  // ------ Constants ------
  inline static auto messages = vec<str>{ "hello", "world", "lorem", "ipsum", "test" };
  inline static auto vars = vec<str>{ "_x", "_y", "_z" };

  // ------ Internal Helpers ------
  /** @brief Adds a generic argument of given type and optional bounds. */
  auto add(str type, uint min=0, uint max=0) -> Signature& { 
    return args.emplace_back(move(type), min, max), *this; 
  }
};
