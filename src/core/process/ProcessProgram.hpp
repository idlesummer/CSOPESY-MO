#include "core/instruction/types.hpp"

namespace csopesy {
  class ProcessProgram {
    using Instructions = vector<Instruction>;
    using Stack = LoopFrame::Stack;

    Instructions instructions;
    Stack context;
    uint ip = 0;

    public:
    ProcessProgram(Instructions inst={}): instructions(move(inst)) {}

    Instructions& get_instructions() { return instructions; }
    const Instructions& get_instructions() const { return instructions; }

    /** Append a new instruction to the program */
    void add_instruction(Instruction inst) {
      instructions.push_back(move(inst));
    }
    
    /** Get number of root instructions */
    size_t size() const {
      return instructions.size();
    }

    Stack& get_context() { return context; }
    const Stack& get_context() const { return context; }

    uint get_ip() const { return ip; }
    void set_ip(uint new_ip) { ip = new_ip; }
    uint next_ip() { return ip++; }

    bool is_finished() const {
      return context.empty() && ip >= instructions.size();
    }
  };
}
