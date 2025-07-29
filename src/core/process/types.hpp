#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"


/** A loop frame from a FOR-like instruction. */
struct ContextFrame {
  uint ip;    // Address where the loop starts
  uint ctr;   // Number of iterations remaining
};

/**
  * @brief A stack structure used for managing nested FOR loop contexts.
  * 
  * Stores control frames for tracking loop entry points, exit conditions, 
  * and iteration counts during process execution. Could also be used to
  * track sleep.
  */
class ContextStack {
  public:

  ContextStack():
    stack (vec<ContextFrame>()) {}

  /** Returns true if the top frame starts at the given instruction pointer. */
  auto top_ip_is(uint ip) -> bool {  return !empty() && top().ip == ip; }

  /** Push a new context onto the stack. */
  void push(uint start, uint count=0) { stack.emplace_back(start, count); }

  /** Pop the top context from the stack. */
  void pop() { stack.pop_back(); }

  /** Check if the context stack is empty. */
  auto empty() -> bool { return stack.empty(); }

  /** Get the number of contexts in the stack. */
  auto size() -> uint { return stack.size(); }

  /** Clear all contexts from the stack. */
  void clear() { stack.clear(); }

  /** Access the top context */
  auto top() -> ContextFrame& { return stack.back(); }

  /** Access a context by index. */
  auto at(uint index) -> ContextFrame& { return stack.at(index); }

  private:

  // ------ Instance variables ------
  vec<ContextFrame> stack;
};
