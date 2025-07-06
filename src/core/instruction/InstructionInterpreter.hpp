#pragma once
#include "core/instruction/handlers/_all.hpp"
#include "core/process/ProcessData.hpp"
#include "InstructionHandler.hpp"
#include "Instruction.hpp"

namespace csopesy {

  /** Central registry and dispatcher for all instruction types. */
  class InstructionInterpreter {
    using Insts = Instruction::list;
    using map  = unordered_map<str, InstructionHandler>;
    using list = vector<ref<const InstructionHandler>>;

    map handlers;           ///< opcode → handler
    list all_handlers;      ///< cached reference list for introspection
    list flat_handlers;     
    list control_handlers;   

    public:

    /** Returns the global singleton instance of the InstructionInterpreter. */
    static InstructionInterpreter& instance() {
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

    Insts generate_instructions(uint count, uint max_depth = 3) {
      using Stack = vector<str>;
      auto insts          = Instruction::list();
      auto control_stack  = Stack();
      uint depth          = 0;

      // for (uint i = 0; i < count; ++i) {
        
      //   // Open new control block
      //   if (!control.empty() && depth < max_depth && Random::chance(5)) {
      //     const auto& handler = *Random::pick(control);
      //     insts.push_back(handler.example(dummy_process));
      //     control_stack.push_back(handler.end_opcode);
      //     ++depth;
      //   }

      //   // Close control block (if any is open)
      //   else if (!control_stack.empty() && Random::chance(10)) {
      //     insts.push_back({ control_stack.back() });
      //     control_stack.pop_back();
      //     --depth;
      //   }

      //   // Add regular flat instruction
      //   else if (!flat.empty()) {
      //     const auto& handler = *Random::pick(flat);
      //     insts.push_back(handler.example(dummy_process));
      //   }
      // }

      // // Ensure all control blocks are properly closed
      // while (!control_stack.empty()) {
      //   insts.push_back({ control_stack.back() });
      //   control_stack.pop_back();
      // }

      return insts;
    }

    private:

    /** 
     * Private constructor to enforce singleton access via instance().
     * Initializes and registers all handlers once. 
     */
    InstructionInterpreter() {
      for (auto& inst : instruction::get_all())
        register_instruction(move(inst));

      // Categorize and cache handler references for quick lookup
      all_handlers.reserve(handlers.size());

      for (const auto& [_, handler]: handlers) {
        all_handlers.push_back(cref(handler));  // Add to full list

        // Separate into control vs. flat based on metadata
        if (handler.is_control())
          control_handlers.push_back(cref(handler));
        else
          flat_handlers.push_back(cref(handler));
      }
    }
  };
}
