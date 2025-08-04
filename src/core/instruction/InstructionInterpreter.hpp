#pragma once
#include "core/instruction/handlers/_all.hpp"
#include "core/process/ProcessData.hpp"
#include "InstructionHandler.hpp"
#include "Instruction.hpp"


/** Central registry and dispatcher for all instruction types. */
class InstructionInterpreter {
  private:
  
  /** 
   * Private constructor to enforce singleton access via get().
   * Initializes and registers all handlers once. 
   */
  InstructionInterpreter():
    handlers         (umap<str,InstructionHandler>()),   // opcode -> handler
    all_handlers     (vec<ref<InstructionHandler>>()),  // cached reference list for introspection
    flat_handlers    (vec<ref<InstructionHandler>>()),  // Handlers for non-control instructions
    control_handlers (vec<ref<InstructionHandler>>())   // Handlers for control instructions
  {
    for (auto& inst: get_instruction_handlers())
      register_instruction(move(inst));

    // Categorize and cache handler references for quick lookup
    all_handlers.reserve(handlers.size());

    for (auto& [_, handler]: handlers) {
      all_handlers.push_back(ref(handler));  // Add to full list

      // Separate into control vs. flat based on metadata
      if (handler.is_control_exit()) continue;
      if (handler.is_control_open())
        control_handlers.push_back(ref(handler));
      else
        flat_handlers.push_back(ref(handler));
    }
  }

  public:

  /** @brief Returns the global singleton get of the InstructionInterpreter. */
  static auto get() -> InstructionInterpreter& {
    static auto inst = InstructionInterpreter();
    return inst;
  }

  /** @brief Registers a handler by opcode. */
  void register_instruction(InstructionHandler handler) {
    handlers[handler.opcode] = move(handler);
  }

  /** @brief Execute an instruction using its handler. */
  void execute(Instruction& inst, ProcessData& proc) {
    auto it = handlers.find(inst.opcode);
    if (it == handlers.end())
      throw runtime_error("Unknown instruction: " + inst.opcode);
    it->second.execute(inst, proc);
  }

  /** @brief Parses a list of tokenized script lines into Instruction objects. */
  auto parse_script(const vec<vec<str>>& token_lines) -> vec<Instruction> {
    auto script = vec<Instruction>();

    for (auto& tokens : token_lines) {
      if (tokens.empty()) continue;

      auto opcode = tokens[0];
      auto it = handlers.find(opcode);
      if (it == handlers.end()) {
        cout << format("[parse_script]: Unknown opcode '{}'\n", opcode);
        return {}; // return empty on unknown instruction
      }

      auto& handler = it->second;
      auto args = vec<str>(tokens.begin()+1, tokens.end());
      auto inst = Instruction(opcode, args);

      if (!handler.matches(inst.args)) {
        cout << format("[parse_script]: Invalid arguments for '{}'\n", opcode);
        return {};
      }
      script.push_back(move(inst));
    }
    return script;
  }

  /** @brief Generates a random list of up to `size` instructions with proper block closure. */
  auto generate_script(uint size, uint max_depth=3) -> vec<Instruction> {
    auto script = vec<Instruction>();
    auto stack = vec<ref<InstructionHandler>>();                      // Tracks opened control blocks
    auto inst_count = [&]() { return script.size() + stack.size(); }; // actual + pending ENDFORS

    while (inst_count() < size) {
      if (should_open(stack, max_depth) && inst_count()+2 <= size)    // For opening control instructions
        open_control_block(script, stack);
      
      else if (should_close(stack))                       // For closing control instructions
        close_control_block(script, stack);

      if (!flat_handlers.empty() && inst_count() < size)  // For adding flat instructions
        script.push_back(Rand::pick(flat_handlers).get().generate());
    }

    // Auto-close any unclosed control blocks
    while (!stack.empty())
      close_control_block(script, stack);

    return script;
  }

  // ------ Instance variables ------

  umap<str,InstructionHandler> handlers;           
  vec<ref<InstructionHandler>> all_handlers;      
  vec<ref<InstructionHandler>> flat_handlers;     
  vec<ref<InstructionHandler>> control_handlers;

  // ------ Internal helpers ------

  /** @brief Returns true if a control block can be opened (depth-limited, random chance). */
  bool should_open(vec<ref<InstructionHandler>>& stack, uint max_depth) { 
    return !control_handlers.empty() && (stack.size() < max_depth) && Rand::chance(4); 
  }

  /** @brief Returns true if a control block can be closed (if any open, random chance). */
  bool should_close(vec<ref<InstructionHandler>>& stack) { 
    return !stack.empty() && Rand::chance(4); 
  }

  /** @brief Emits a random control-opener instruction and pushes it to the stack. */
  void open_control_block(vec<Instruction>& script, vec<ref<InstructionHandler>>& stack) {
    auto& handler = Rand::pick(control_handlers);
    script.push_back(handler.get().generate());
    stack.push_back(handler);
  }

  /** @brief Emits the matching end-opcode of the current open control block. */
  void close_control_block(vec<Instruction>& script, vec<ref<InstructionHandler>>& stack) {
    if (stack.empty())
      throw runtime_error("Attempted to emit ENDFOR with empty control stack!");
    
    auto& opener = stack.back();
    script.emplace_back(opener.get().exit_opcode);
    stack.pop_back();
  }
};
