#pragma once
#include "core/instruction/Instruction.hpp"
#include "types.hpp"

namespace csopesy {

  /** A program that can be executed by a process. */
  class ProcessProgram {
    public:
    using list = vector<Instruction>;
    
    private:
    using Stack = ContextStack;

    list insts;  ///< Flat list of all program instructions
    Stack context;  ///< Stack of active loop contexts
    uint ip = 0;    ///< Current instruction pointer

    public:

    /** Construct a process program with an optional instruction list. */
    ProcessProgram(list inst={}): insts(move(inst)) {}

    /** Append a new instruction to the program */
    void add_instruction(Instruction inst) {
      insts.push_back(move(inst));
    }

    /** Get the total number of instructions in the program. */
    uint size() const {
      return insts.size();
    }

    /** Access and control the instruction pointer. */
    uint get_ip() const { return ip; }
    void set_ip(uint new_ip) { ip = new_ip; }
    uint next_ip() { return ip + 1; }

    /** Check if the program has completed execution. */
    bool is_finished() const {
      return ip >= insts.size();
    }

    /** Access the loop context stack. */
    Stack& get_context() { return context; }
    const Stack& get_context() const { return context; }

    /** Access list of all instructions. */
    list& get_instructions() { return insts; }
    const list& get_instructions() const { return insts; }
  };
}
