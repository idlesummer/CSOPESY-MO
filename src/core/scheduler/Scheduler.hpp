#pragma once
#include "core/common/imports/_all.hpp"
#include "core/process/Process.hpp"
#include "core/instruction/InstructionInterpreter.hpp"
#include "types.hpp"

namespace csopesy {
  class Scheduler {
    using Interpreter = InstructionInterpreter;
    using ProcList    = List<Process>;
    using ProcRefList = vector<ref<const Process>>;
    using CoreList    = vector<optional<ref<Process>>>;

    ProcList processes;
    ProcRefList finished;
    queue<ref<Process>> rqueue;
    CoreList cores;
    SchedulerConfig config;
    uint tick_count = 0;
    bool generating = false;
    uint next_process_id = 1;
    Interpreter& interpreter;

    public:

    Scheduler(): interpreter(Interpreter::instance()) {}

    /** Add a process to the global process list */
    void add_process(Process proc) {
      processes.push_back(move(proc));
      rqueue.push(ref(processes.back()));
    }

    /** Advance 1 CPU tick */
    void tick() {
      ++tick_count;

      // Dummy generation
      if (generating && tick_count % config.batch_process_freq == 0)
        create_dummy_process();

      if (config.scheduler == "fcfs")
        tick_fcfs();
      else
        return; 
      tick_rr();
    }

    void tick_fcfs() {
      // === Phase 1: Assign processes to idle cores
      
      for (uint i = 0; i < cores.size(); ++i) {
        auto& core = cores[i];

        // Skip if this core is already running something
        if (core.has_value()) continue;

        // Skip if no ready process available
        while (!rqueue.empty()) {
          auto proc_ref = rqueue.front(); rqueue.pop();
          auto& proc = proc_ref.get();

          // Sanity check: skip if process is already finished
          if (proc.get_state().is_finished())
            continue;

          // Sanity check: should not be assigned to a core already
          if (proc.get_core() != -1)
            continue;

          // Assign to this core
          core = proc_ref;
          proc.set_core(static_cast<int>(i));
          break;
        }
      }

      // === Phase 2: Step all active cores
      for (uint i = 0; i < cores.size(); ++i) {
        auto& core = cores[i];
        if (!core.has_value()) continue;

        auto& proc = core->get();

        // Sanity check: core mismatch
        if (proc.get_core() != static_cast<int>(i)) {
          proc.log(format("[tick] ERROR: Core mismatch — process p{:02} claims core {}, but is on core {}",
                          proc.get_pid(), proc.get_core(), i));
        }

        // Step the process
        step_process(proc);

        // If the process finished, release the core
        if (proc.get_state().is_finished()) {
          proc.log(format("[tick] finished on core {}", i));
          proc.reset_core();
          core.reset();
        }
      }
    }

    void tick_rr() {
      // Round Robin: assign cores in a round-robin fashion
      for (uint i = 0; i < cores.size(); ++i) {
        auto& core = cores[i];

        if (!core.has_value()) {
          if (!rqueue.empty()) {
            core = rqueue.front();
            rqueue.pop();
          }
          continue;
        }

        auto& proc = core.value().get();
        step_process(proc);

        if (proc.get_state().state == State::Finished) {
          core.reset();
        } else {
          // Re-queue the process if it is still running
          rqueue.push(ref(proc));
        }
      }
    }

    /** Execute a single instruction for one process */
    void step_process(Process& proc) {
      if (proc.step())
        finished.push_back(cref(proc));
    }

    /** Generate one dummy process */
    void create_dummy_process() {
      auto name = format("p{:02}", next_process_id);
      auto proc = Process(name, next_process_id++);
     
      // Randomly generate a script of a random instruction count
      uint size = Random::num(config.min_ins, config.max_ins);
      auto script = interpreter.generate_script(size);

      // Load the script into the program
      proc.get_program().load_script(move(script));

      // Add process to process list
      add_process(move(proc));
    }

    void start_generation() { generating = true; }
    void stop_generation()  { generating = false; }
    bool is_generating() const { return generating; }

    const ProcList& get_processes() const { return processes; }
    const Process& get_last_process() const { return processes.back(); }
    const ProcRefList& get_finished_processes() const { return finished; }

    vector<ref<const Process>> get_running_processes() const {
      vector<ref<const Process>> running;
      for (const auto& proc : processes) {
        if (!proc.get_program().is_finished())
          running.push_back(cref(proc));
      }
      return running;
    }

    bool all_finished() const {
      for (const auto& proc : processes)
        if (!proc.get_program().is_finished())
          return false;
      return true;
    }

    SchedulerConfig& get_config() { return config; }
    const SchedulerConfig& get_config() const { return config; }

    void set_config(SchedulerConfig new_config) {
      config = move(new_config);

      // Resize core list to match num_cpu, all cores unassigned (nullopt)
      cores = CoreList(config.num_cpu, std::nullopt);

      // Resize per-core quantum tracking for RR (default 0)
      // core_quantums = vector<uint>(config.num_cpu, 0);

      // Optional debug log
      cout << format("[config] Initialized {} CPU cores.\n", config.num_cpu);
    }

    uint get_tick_count() const { return tick_count; }
    bool is_initialized() const { return config.initialized; }
  };
}
