#pragma once
#include "core/instruction/handlers/_all.hpp"
#include "core/process/Process.hpp"
#include "types.hpp"

namespace csopesy {

  /**
   * @brief Central registry and dispatcher for all instruction types.
   */
  class InstructionInterpreter {
    using map  = unordered_map<str, InstructionHandler>;
    using list = vector<ref<const InstructionHandler>>;

    map handlers;             ///< opcode → handler
    list handler_list;        ///< cached reference list for introspection

  public:
    /** Initializes and registers all handlers once. */
    InstructionInterpreter() {
      for (auto& inst : instruction::get_all())
        register_instruction(move(inst));

      handler_list.reserve(handlers.size());
      for (const auto& [_, handler] : handlers)
        handler_list.push_back(cref(handler));
    }

    /** Registers a handler by opcode. */
    void register_instruction(InstructionHandler handler) {
      handlers[handler.opcode] = move(handler);
    }

    /** Executes an instruction using its mapped handler. */
    void execute(const Instruction& inst, Process& proc) const {
      auto it = handlers.find(inst.opcode);
      if (it == handlers.end())
        throw runtime_error("Unknown instruction: " + inst.opcode);

      it->second.execute(proc, inst);
    }

    /** Returns list of all registered handlers. */
    const list& get_handler_list() const {
      return handler_list;
    }

    /** Returns the singleton instance. */
    static InstructionInterpreter& instance() {
      static InstructionInterpreter inst;
      return inst;
    }
  };
}
