#pragma once
#include "core/instruction/Instruction.hpp"
#include "types.hpp"

  
/**
 * @brief A program that can be executed by a process.
 * 
 * Contains a flat list of instructions, an instruction pointer,
 * and a control context stack (used by FOR loops).
 */
class ProcessProgram {
  public:

  /** @brief Loads an instruction script. */
  ProcessProgram(vec<Instruction> script): 
    script     (vec<Instruction>(move(script))),  // Flat list of all program instructions
    context    (ContextStack()),                     // Stack of active loop contexts
    ip_was_set (false),                              // For blocking instruction pointer advancing
    ip         (0) {}                                // Current instruction pointer

  /** @brief Returns the size of the script. */
  auto size() -> uint { return script.size(); }
  
  /** @brief Check if the program has completed execution. */
  auto finished() -> bool { return ip >= script.size(); }

  /** @brief Manually sets the instruction pointer to a specific address. */
  void set_ip(uint addr) {
    ip_was_set = true;
    ip = addr;
  }

  /** @brief Returns a formatted view of all instructions with the current IP highlighted. */
  auto render_script() -> str {
    // Compute the width needed to align inst indices
    uint width = count_digits(script.size()-1);
    auto out = osstream();
    
    // Render each instruction line, marking the current IP with a '>'
    for (uint i=0; i < script.size(); ++i)
      out << render_line(i, width);

    return out.str();
  }

  /** @brief Returns a formatted view of the current context stack. */
  auto render_context() -> str {
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

  // ------ Instance variables ------

  vec<Instruction> script;
  ContextStack context;
  bool ip_was_set;
  uint ip;

  // ------ Internal logic ------

  private:

  /** @brief Helper to renders a single instruction line from the script with formatting. */
  auto render_line(uint idx, uint width) -> str {
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
  auto render_frame(uint idx, uint width) -> str {
    auto& frame = context.at(idx);
    auto& inst = script[frame.ip];
    auto out = osstream();

    out << format("  [{}]", idx);
    out << format(" @{:0{}}", frame.ip, width);
    out << format("  exit: {:0{}}", inst.exit, width);
    out << format("  count: {}", frame.ctr);
    out << '\n';
    return out.str();
  }
};
