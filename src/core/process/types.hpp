#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"


/** A loop frame from a FOR-like instruction. */
struct ContextFrame {
  str  opcode;  // Control frame opcode, e.g., "FOR"
  uint start;   // Address where the loop starts
  uint count;   // Number of iterations remaining
};

/**
  * @brief A stack structure used for managing nested FOR loop contexts.
  * 
  * Stores control frames for tracking loop entry points, exit conditions, 
  * and iteration counts during process execution.
  */
class ContextStack {
  public:

  ContextStack():
    stack (vector<ContextFrame>()) {}
  
  /** Checks if top frame matches the opcode. */
  auto matches(const str& opcode) -> bool { 
    return !empty() && top().opcode == opcode; 
  }

  /** Checks if top frame matches the opcode, start address, and has an initialized exit address. */
  auto matches(const str& opcode, uint start) -> bool {
    return !empty() && top().opcode == opcode && top().start == start;
  }

  /** Returns true if the top frame starts at the given instruction pointer. */
  auto starts_at(uint ip) -> bool { 
    return !empty() && top().start == ip; 
  }

  /** Push a new loop context onto the stack. */
  void push(str opcode, uint start, uint count) {
    stack.emplace_back(move(opcode), start, count);
  }

  /** Pop the top loop context from the stack. */
  void pop() { stack.pop_back(); }

  /** Check if the loop context stack is empty. */
  auto empty() -> bool { return stack.empty(); }

  /** Get the number of loop contexts in the stack. */
  auto size() -> uint { return stack.size(); }

  /** Clear all loop contexts from the stack. */
  void clear() { stack.clear(); }

  /** Access the top loop context */
  auto top() -> ContextFrame& { return stack.back(); }

  /** Access a loop context by index. */
  auto at(uint index) -> ContextFrame& { return stack.at(index); }

  private:

  // ------ Member variables ------
  vector<ContextFrame> stack;
};
