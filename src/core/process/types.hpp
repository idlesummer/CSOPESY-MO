#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"

namespace csopesy {

  /** An enum representing the possible states of a process. */
  enum class State {
    Ready, Sleeping, Finished,
  };

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
    using Stack = vector<ContextFrame>;
    
    Stack stack;

    public:
    
    /** Checks if top frame matches the opcode. */
    bool matches(const str& opcode) const { return !empty() && top().opcode == opcode; }

    /** Checks if top frame matches the opcode, start address, and has an initialized exit address. */
    bool matches(const str& opcode, uint start) const {
      return !empty() && top().opcode == opcode && top().start == start;
    }

    /** Returns true if the top frame starts at the given instruction pointer. */
    bool starts_at(uint ip) const { return !empty() && top().start == ip; }

    /** Push a new loop context onto the stack. */
    void push(str opcode, uint start, uint count) {
      stack.emplace_back(move(opcode), start, count);
    }

    /** Pop the top loop context from the stack. */
    void pop() { stack.pop_back(); }

    /** Check if the loop context stack is empty. */
    bool empty() const { return stack.empty(); }

    /** Get the number of loop contexts in the stack. */
    uint size() const { return stack.size(); }

    /** Clear all loop contexts from the stack. */
    void clear() { stack.clear(); }

    /** Access the top loop context */
    ContextFrame& top() { return stack.back(); }
    const ContextFrame& top() const { return stack.back(); }

    /** Access a loop context by index. */
    ContextFrame& at(uint index) { return stack.at(index); }
    const ContextFrame& at(uint index) const { return stack.at(index); }
  };
}
