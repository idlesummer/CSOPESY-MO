#pragma once
#include "core/instruction/Instruction.hpp"
#include "types.hpp"

namespace csopesy {
  
  /**
   * @brief A program that can be executed by a process.
   * 
   * Contains a flat list of instructions, an instruction pointer,
   * and a control context stack (used by FOR loops).
   */
  class ProcessProgram {
    using Script = Instruction::Script;
    using Stack = ContextStack;

    public:
    Script script;  ///< Flat list of all program instructions
    Stack context;  ///< Stack of active loop contexts
    uint ip = 0;    ///< Current instruction pointer

    // === Methods ===

    /** @brief Loads a new instruction script and resets execution state. */
    void load_script(Script new_script) { 
      script = move(new_script);
      context.clear();
      ip = 0;  
    }
    
    /** @brief Returns the size of the script. */
    uint size() const { return script.size(); }
    
    /** @brief Check if the program has completed execution. */
    bool finished() const { return ip >= script.size(); }

    /** @brief Returns a formatted view of all instructions with the current IP highlighted. */
    str view_script() const {
      auto stream = osstream();

      // Compute padding width based on the number of instructions
      uint addr_width = to_string(script.size()-1).length();
      
      for (uint i=0; i < script.size(); ++i) {
        const auto& inst = script[i];
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

    /** @brief Returns a formatted view of the current context stack. */
    str view_context() const {
      auto stream = osstream();

      if (context.empty())
        return stream << "  <empty>\n",  stream.str();

      // Compute padding width based on the number of instructions
      uint width = to_string(script.size()-1).length();

      for (uint i=0; i < context.size(); ++i) {
        const auto& frame = context.at(i);
        const auto& inst  = script[frame.start];

        stream << format("  [{}] {:<6} @{:0{}}  exit: {:0{}}  count: {}\n",
          i, frame.opcode, frame.start, width, inst.exit, width, frame.count);
      }

      return move(stream).str();
    }
  };
}
