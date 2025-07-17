#pragma once
#include "core/instruction/handlers/_all.hpp"
#include "core/process/ProcessData.hpp"
#include "InstructionHandler.hpp"
#include "Instruction.hpp"


/** Central registry and dispatcher for all instruction types. */
class InstructionInterpreter {
  using Script = Instruction::Script;
  using map  = unordered_map<str, InstructionHandler>;
  using list = vector<ref<const InstructionHandler>>;

  map handlers;           ///< opcode → handler
  list all_handlers;      ///< cached reference list for introspection
  list flat_handlers;     ///< Handlers for non-control instructions
  list control_handlers;  ///< Handlers for control instructions

  public:

  /** Returns the global singleton get of the InstructionInterpreter. */
  static InstructionInterpreter& get() {
    static InstructionInterpreter inst;
    return inst;
  }

  /** Registers a handler by opcode. */
  void register_instruction(InstructionHandler handler) {
    handlers[handler.opcode] = move(handler);
  }

  /** Execute an instruction using its handler. */
  void execute(const Instruction& inst, ProcessData& proc) const {
    auto it = handlers.find(inst.opcode);
    if (it == handlers.end())
      throw runtime_error("Unknown instruction: " + inst.opcode);

    it->second.execute(inst, proc);
  }

  /** Returns list of registered handlers. */
  const list& get_handlers() const { return all_handlers; }
  const list& get_flat_handlers() const { return flat_handlers; }
  const list& get_control_handlers() const { return control_handlers; }

  /**
   * Generates a random list of up to `size` instructions with proper block closure.
   * May exceed size due to necessary ENDFOR-like closures.
   */
  Script generate_script(uint size, uint max_depth=3) {
    auto script = Script();
    list stack; // Tracks opened control blocks

    while (script.size() < size) {
      if (should_open(stack, max_depth))  // For opening control instructions
        open_control_block(script, stack);
      
      else if (should_close(stack))       // For closing control instructions
        close_control_block(script, stack);

      if (!flat_handlers.empty())                 // For adding flat instructions
        script.push_back(Rand::pick(flat_handlers).get().generate());
    }

    // Auto-close any unclosed control blocks
    while (!stack.empty())
      close_control_block(script, stack);

    return script;
  }

  private:

  /** 
   * Private constructor to enforce singleton access via get().
   * Initializes and registers all handlers once. 
   */
  InstructionInterpreter() {
    for (auto& inst: get_instruction_handlers())
      register_instruction(move(inst));

    // Categorize and cache handler references for quick lookup
    all_handlers.reserve(handlers.size());

    for (const auto& [_, handler]: handlers) {
      all_handlers.push_back(cref(handler));  // Add to full list

      // Separate into control vs. flat based on metadata
      if (handler.is_control_exit()) continue;
      if (handler.is_control_open())
        control_handlers.push_back(cref(handler));
      else
        flat_handlers.push_back(cref(handler));
    }
  }

  // === Internal Helpers ===

  /** Returns true if a control block can be opened (depth-limited, random chance). */
  bool should_open(const list& stack, uint max_depth) const { 
    return !control_handlers.empty() && (stack.size() < max_depth) && Rand::chance(4); 
  }

  /** Returns true if a control block can be closed (if any open, random chance). */
  bool should_close(const list& stack) const { 
    return !stack.empty() && Rand::chance(4); 
  }

  /** Emits a random control-opener instruction and pushes it to the stack. */
  void open_control_block(Script& script, list& stack) const {
    const auto& handler = Rand::pick(control_handlers);
    script.push_back(handler.get().generate());
    stack.push_back(handler);
  }

  /** Emits the matching end-opcode of the current open control block. */
  void close_control_block(Script& script, list& stack) const {
    if (stack.empty())
      throw runtime_error("Attempted to emit ENDFOR with empty control stack!");
    
    const auto& opener = stack.back();
    script.emplace_back(opener.get().exit_opcode);
    stack.pop_back();
  }
};
