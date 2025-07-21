#pragma once
#include "core/common/imports/_all.hpp"


/** @brief Represents one argument in an instruction signature. */
class Arg {
  public:

  Arg(str type, uint min=0, uint max=0): 
    type (move(type)),  // Parameter type: "str", "uint", or "var"
    min  (min),         // Minimum value (for "uint" type)
    max  (max) {}       // Maximum value (for "uint" type)
  
  // ------ Member variables ------
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
  auto strT() -> Signature& { return add("str"); }

  /** @brief Adds a variable name argument. */
  auto varT() -> Signature& { return add("var"); }

  /** @brief Adds an unsigned integer argument with optional bounds. */
  auto uintT(uint min=0, uint max=65535) -> Signature& { return add("uint", min, max); }

  /** @brief Adds a generic argument of given type and optional bounds. */
  auto add(str type, uint min=0, uint max=0) -> Signature& { return args.emplace_back(move(type), min, max), *this; }

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

  private:

  // ------ Member variables ------
  vec<Arg> args;

  // ------ Constants ------
  inline static  auto messages = vec<str>{ "hello", "world", "lorem", "ipsum", "test" };
  inline static  auto vars = vec<str>{ "x", "y", "z" };
};
