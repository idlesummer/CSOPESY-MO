#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"

namespace csopesy {

  /** An enum representing the possible states of a process. */
  enum class State {
    Ready,
    Sleeping,
    Finished,
  };

  /** A loop frame from a FOR-like instruction. */
  struct ContextFrame {
    str  name;      // optional label for debugging
    uint line_addr; // where the FOR begins
    uint jump_addr; // where to go when the loop ends
    uint count;     // remaining repeat count
  };

  class ContextStack {
    using Stack = vector<ContextFrame>;
    Stack stack;

    public:

    /** Check if the loop context stack is empty. */
    bool empty() const { 
      return stack.empty(); 
    }

    /** Get the number of loop contexts in the stack. */
    uint size() const { 
      return stack.size(); 
    }
    
    /** Push a new loop context onto the stack. */
    void push(str name, uint line_addr, uint jump_addr, uint count) {
      stack.emplace_back(move(name), line_addr, jump_addr, count);
    }

    /** Pop the top loop context from the stack. */
    void pop() { stack.pop_back(); }

    /** Set the jump address for the top loop context. */
    void set_jump(uint addr) {
      if (!stack.empty()) 
        stack.back().jump_addr = addr;
    }

    /** Check whether the program is currently inside a loop. */
    bool in_loop() const { 
      return !stack.empty(); 
    }

    /** Clear all loop contexts from the stack. */
    void clear() { 
      stack.clear(); 
    }

    /** Get a reference to the raw stack of loop contexts. */
    const Stack& raw() const {
      return stack; 
    }

    /** Access the top loop context */
    ContextFrame& top() { return stack.back(); }
    const ContextFrame& top() const { return stack.back(); }
  };
}
