#pragma once
#include "core/instruction/handlers/_all.hpp"
#include "core/process/ProcessData.hpp"
#include "InstructionHandler.hpp"

namespace csopesy {

  /** Central registry and dispatcher for all instruction types. */
  class InstructionInterpreter {
    using map  = unordered_map<str, InstructionHandler>;
    using list = vector<ref<const InstructionHandler>>;

    map handlers;       ///< opcode → handler
    list handler_list;  ///< cached reference list for introspection

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

    /** Executes an instruction using its mapped handler. */
    Uint execute(const Instruction& inst, ProcessData& proc) const {
      auto it = handlers.find(inst.opcode);
      if (it == handlers.end())
        throw runtime_error("Unknown instruction: " + inst.opcode);

      return it->second.execute(inst, proc);
    }

    /** Returns list of all registered handlers. */
    const list& get_handler_list() const {
      return handler_list;
    }

    private:

    /** 
     * Private constructor to enforce singleton access via instance().
     * Initializes and registers all handlers once. 
     */
    InstructionInterpreter() {
      for (auto& inst : instruction::get_all())
        register_instruction(move(inst));

      handler_list.reserve(handlers.size());
      for (const auto& [_, handler]: handlers)
        handler_list.push_back(cref(handler));
    }
  };
}
