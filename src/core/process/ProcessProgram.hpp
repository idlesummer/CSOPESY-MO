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
    Script script = {}; // Flat list of all program instructions
    Stack context = {}; // Stack of active loop contexts
    uint ip = 0;        // Current instruction pointer

    // === Methods ===
    
    /** @brief Loads an instruction script. */
    ProcessProgram(Script script): script(move(script)) {}

    /** @brief Returns the size of the script. */
    uint size() const { return script.size(); }
    
    /** @brief Check if the program has completed execution. */
    bool finished() const { return ip >= script.size(); }

    /** @brief Returns a formatted view of all instructions with the current IP highlighted. */
    str render_script() const {
      // Compute the width needed to align inst indices
      uint width = count_digits(script.size()-1);
      auto out = osstream();
      
      // Render each instruction line, marking the current IP with a '>'
      for (uint i=0; i < script.size(); ++i)
        out << render_line(i, width);

      return out.str();
    }

    /** @brief Returns a formatted view of the current context stack. */
    str render_context() const {
      // If there are no loop frames, return empty
      if (context.empty()) 
        return "  <empty>\n";  
      
      // Compute width for formatting inst indices
      uint width = count_digits(script.size()-1); 
      auto out = osstream();

      // Render each loop frame (used by nested FOR instructions)
      for (uint i=0; i < context.size(); ++i)
        out << render_frame(i, width);

      return out.str();
    }

    // ========================
    // === Private Members ====
    // ========================
  private:

    /** @brief Helper to renders a single instruction line from the script with formatting. */
    str render_line(uint idx, uint width) const {
      char marker = (idx == ip) ? '>' : ' ';
      auto opcode = script[idx].opcode.substr(0, 10);
      auto line = osstream();
      line << format("{} [{:0{}}] {:<{}}", marker, idx, width, opcode, 10);

      for (auto& arg: script[idx].args)
        line << ' ' << arg;
      line << '\n';

      return line.str();
    }

    /** @brief Helper to renders a single loop frame from the context stack with formatting. */
    str render_frame(uint idx, uint width) const {
      auto& frame = context.at(idx);
      auto& inst = script[frame.start];
      auto out = osstream();

      out << format("  [{}] {:<6}", idx, frame.opcode);
      out << format(" @{:0{}}", frame.start, width);
      out << format("  exit: {:0{}}", inst.exit, width);
      out << format("  count: {}", frame.count);
      out << '\n';

      return out.str();
    }
  };
}
