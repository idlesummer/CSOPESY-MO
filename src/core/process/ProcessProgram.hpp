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
    void add_instruction(Instruction inst) { insts.push_back(move(inst)); }

    /** Returns a reference to the instruction at the given address. */
    const Instruction& get_instruction(uint ip) { return insts.at(ip); }

    /** Access list of all instructions. */
    list& get_instructions() { return insts; }
    const list& get_instructions() const { return insts; }
    
    /** Get the total number of instructions in the program. */
    uint size() const { return insts.size(); }

    /** Access and control the instruction pointer. */
    uint get_ip() const { return ip; }
    void set_ip(uint new_ip) { ip = new_ip; }

    /** Check if the program has completed execution. */
    bool is_finished() const { return ip >= insts.size(); }

    /** Access the loop context stack. */
    Stack& get_context() { return context; }
    const Stack& get_context() const { return context; }

    /** Returns a formatted view of all instructions with the current IP highlighted. */
    str view_instructions() const {
      auto stream = osstream();

      // Compute padding width based on the number of instructions
      uint addr_width = to_string(insts.size()-1).length();
      
      for (uint i=0; i < insts.size(); ++i) {
        const auto& inst = insts[i];
        const auto marker = (i == ip) ? '>' : ' ';
        const uint opcode_width = 10;
        auto opcode = inst.opcode;

        if (opcode.size() > opcode_width)
          opcode = opcode.substr(0, opcode_width);

        stream << format("{} [{:0{}}] {:<{}}", 
          marker, i, addr_width, opcode, opcode_width);

        for (const auto& arg: inst.args)
          stream << ' ' << arg;
        stream << '\n';
      }

      return move(stream).str();
    }

    /** Returns a formatted view of the current context stack. */
    str view_context() const {
      auto stream = osstream();

      if (context.empty())
        return stream << "  <empty>\n",  stream.str();

      // Compute padding width based on the number of instructions
      uint width = to_string(insts.size()-1).length();

      for (uint i=0; i < context.size(); ++i) {
        const auto& frame = context.at(i);
        const auto& inst  = insts[frame.start];

        stream << format("  [{}] {:<6} @{:0{}}  exit: {:0{}}  count: {}\n",
          i, frame.opcode, frame.start, width, inst.exit, width, frame.count);
      }

      return move(stream).str();
    }
  };
}
