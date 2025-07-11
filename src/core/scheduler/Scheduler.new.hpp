#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/InstructionInterpreter.hpp"
#include "core/process/Process.hpp"
#include "core/execution/Core.hpp"
#include "strategies/_all.hpp"
#include "SchedulerStrategy.hpp"
#include "SchedulerData.hpp"
#include "types.hpp"

namespace csopesy {

  /**
   * @brief Central scheduler controller for the OS simulation.
   * 
   * Owns the strategy, configuration, interpreter, and high-level
   * orchestration of ticks and process generation.
   */
  class Scheduler {
    using Interpreter = InstructionInterpreter;
    using ProcessRef = Core::ProcessRef;
    using map = unordered_map<str, SchedulerStrategy>;
    using list = vector<str>;

    // === State ===
    uint ticks = 0;           ///< Global tick counter
    bool generating = false;  ///< Flag indicating auto-generation mode
    list proc_queue;          ///< Deferred generation queue
    
    // === Components ===
    Interpreter& interpreter; ///< Shared instruction generator instance
    SchedulerData data;       ///< Internal state (cores, processes, queue)
    map strategies;           ///< Map containing scheduler strategies

    public:

    /** Registers all available scheduling strategies. */
    Scheduler(): interpreter(Interpreter::instance()) {
      for (auto& strat: scheduler::get_all())
        strategies.emplace(strat.get_name(), move(strat));
    }

    /** Executes the active strategy logic and increments the tick count. */
    void tick() {
      
      // Handle any queued process generations
      for (auto& name: proc_queue)
        generate_process(move(name));
      proc_queue.clear();
      
      // Generate process every `batch_process_freq` ticks
      if (generating && interval_has_elapsed())
        generate_process();

      // get_strategy().preempt(data);
      get_strategy().tick(data);
      ++ticks;
    }

    /** Applies a new configuration and resizes core state accordingly. */
    void set_config(SchedulerConfig config) {
      if (!strategies.contains(config.scheduler))
        throw runtime_error(format("Unknown scheduler strategy: {}", config.scheduler));

      data.set_config(move(config));

      // Hook up core event listeners for running/finished tracking
      for (auto& ref: data.get_cores().get_all()) {
        auto& core = ref.get();
        core.on("assign", [this](any payload) { on_core_assign(payload); });
        core.on("release", [this](any payload) { on_core_release(payload); });
      }
    }    
    
    // === Generation Control ===
    void enqueue_process(str name) { proc_queue.push_back(move(name)); }
    void generate(bool flag) { generating = flag; }
    bool is_generating() const { return generating; }

    // === Accessors ===
    uint get_ticks() const { return ticks; }
    bool is_initialized() const { return data.get_config().initialized; }
    
    // === Component Accessors ===
    SchedulerData& get_data() { return data; }
    SchedulerConfig& get_config() { return data.get_config(); }
    SchedulerStrategy& get_strategy() { return strategies.at(data.get_config().scheduler); }
    const SchedulerData& get_data() const { return data; }
    const SchedulerConfig& get_config() const { return data.get_config(); }
    const SchedulerStrategy& get_strategy() const { return strategies.at(data.get_config().scheduler); }

    private:

    // === Helper methods ===

    /** Helper that returns true if the current tick matches the process generation interval. */
    bool interval_has_elapsed() const {
      uint freq = data.get_config().batch_process_freq;
      return freq > 0 && (ticks % freq == 0);
    }

    /** Internal helper that generates a process with optional name. */
    uint generate_process(Str name=nullopt) { 
      // Instantiate the process, reserve 0 for main, so start PIDs from 1
      uint pid = data.generate_pid();
      str new_name = name.value_or(format("p{:02}", pid));
      auto proc = Process(move(new_name), pid);
      
      // Randomly generate a script with random instructions and load into program
      auto& config = data.get_config();
      auto script = interpreter.generate_script(Random::num(config.min_ins, config.max_ins));
      proc.get_program().load_script(move(script));

      // Add to table first so it's owned and safe to reference
      data.add_process(move(proc));

      // Now call strategy with the stored instance
      get_strategy().add(data.get_process(pid).get(), data);
      return pid;
    }

    /** Tracks the process as running when it is assigned to a core. */
    void on_core_assign(any payload) {
      auto& proc = cast<ProcessRef&>(payload).get();
      data.get_running().insert(proc.get_id());
    }

    /** Tracks the process as finished when it is released from a core. */
    void on_core_release(any payload) {
      auto& proc = cast<ProcessRef&>(payload).get();
      data.get_running().erase(proc.get_id());
      data.get_finished().push_back(proc.get_id());
    }
  };
}
