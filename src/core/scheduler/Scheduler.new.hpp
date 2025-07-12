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
    using queue = vector<str>;
    using list = vector<uint>;

    uint quantum_counter = 1;

    // === State ===
    uint ticks = 0;             ///< Global tick counter
    bool generating = false;    ///< Flag indicating auto-generation mode
    queue proc_queue;           ///< Deferred generation queue
    
    // === Components ===
    Interpreter& interpreter;   ///< Shared instruction generator instance
    SchedulerData data;         ///< Internal state (cores, processes, queue)
    SchedulerStrategy strategy; ///< Contains the scheduler strategy

    public:

    /** Registers all available scheduling strategies. */
    Scheduler(): 
      interpreter(Interpreter::instance()), 
      strategy(scheduler::make_strategy("fcfs", SchedulerConfig())){}

    /** Executes the active strategy logic and increments the tick count. */
    void tick() {

      // 1. Generate any explicitly enqueued processes
      for (auto& name : proc_queue) {
        if (generate_process(move(name)) == MAX)
          cout << "[Scheduler] Skipped due to memory limit\n";
      }
      proc_queue.clear();
      
      // 2. Possibly auto-generate processes this tick
      if (generating && interval_has_elapsed())
        generate_process();

      // 3. Update running/finished state and handle preemption
      auto& running = data.get_running();
      auto& finished = data.get_finished();
      running.clear();

      for (auto& ref : data.get_cores().get_all()) {
        auto& core = ref.get();
        bool idle = core.is_idle();

        if (!idle)
          running.insert(core.get_job().get_id());

        if (!idle && core.wants_release()) {
          auto& proc = core.get_job();
          core.release();

          if (proc.get_state().is_finished()){
            data.get_memory().deallocate(proc.get_id());
            finished.push_back(proc.get_id());
          }
          else
            data.get_rqueue().push(proc.get_id());
        }
      }

      // // Debug: Show current running and finished sets
      // cout << format("[Tick {}] Running PIDs: ", ticks);
      // for (auto pid : running) cout << pid << " ";
      // cout << "\n";

      // cout << format("[Tick {}] Finished PIDs: ", ticks);
      // for (auto pid : finished) cout << pid << " ";
      // cout << "\n";

      // 4. Schedule ready processes to idle cores
      strategy.tick(data);
      ++ticks;

      uint quantum = data.get_config().quantum_cycles;
      if (quantum > 0 && ticks % quantum == 0) {
        dump_memory_snapshot(data, quantum_counter++);
      }



    }

    /** Applies a new configuration and resizes core state accordingly. */
    void set_config(SchedulerConfig config) {
      // 1. Store config inside SchedulerData
      data.set_config(config);

      // 2. Build and install the selected strategy
      strategy = scheduler::make_strategy(config.scheduler, config);

      // 3. Inject per-core preemption policy from strategy
      if (strategy.get_preemption_policy()) {
        for (auto& ref : data.get_cores().get_all()) {
          ref.get().set_preemption_policy(strategy.get_preemption_policy());
        }
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
    const SchedulerData& get_data() const { return data; }
    const SchedulerConfig& get_config() const { return data.get_config(); }

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

      if (!data.get_memory().allocate(pid)) {
        return MAX;
      }

      str new_name = name.value_or(format("p{:02}", pid));
      auto proc = Process(move(new_name), pid);
      
      // Randomly generate a script with random instructions and load into program
      auto& config = data.get_config();
      auto script = interpreter.generate_script(Random::num(config.min_ins, config.max_ins));
      proc.get_program().load_script(move(script));

      // Add to table first so it's owned and safe to reference
      data.add_process(move(proc));

      // Enqueue by PID
      data.get_rqueue().push(pid);
      return pid;
    }

    void dump_memory_snapshot(const csopesy::SchedulerData& data, uint quantum_count) {
      using namespace std;
      using namespace csopesy;

      const auto& mem = data.get_memory();
      const auto& blocks = mem.get_blocks();

      // Get current timestamp
      auto now = chrono::system_clock::now();
      auto time = chrono::system_clock::to_time_t(now);
      stringstream timestamp;
      timestamp << put_time(localtime(&time), "%m/%d/%Y %I:%M:%S %p");

      // Start file output
      string filename = format("memory_stamp_{}.txt", quantum_count);
      ofstream out(filename);
      if (!out) {
        cerr << "[error] Could not write to " << filename << "\n";
        return;
      }

      out << "Timestamp: " << timestamp.str() << "\n";
      out << "Number of processes in memory: " << mem.count_active() << "\n";
      out << "Total external fragmentation in KB: " << mem.external_fragmentation() / 1024 << "\n";

      out << "---end--- = " << 16384 << "\n" << "\n";

      // Print process blocks top-down
      for (auto it = blocks.rbegin(); it != blocks.rend(); ++it) {
        const auto& b = *it;
        if (b.pid != -1) {
          out << b.start + b.size << "\n";
          out << "P" << b.pid << "\n";
          out << b.start << "\n" <<"\n";
        }
      }

      out << "---start--- = 0\n";
    }

    // void dump_memory_snapshot(const csopesy::SchedulerData& data, uint quantum_count) {
    //   using namespace std;
    //   using namespace csopesy;

    //   const auto& mem = data.get_memory();
    //   const auto& blocks = mem.get_blocks();

    //   // Get current timestamp
    //   auto now = chrono::system_clock::now();
    //   auto time = chrono::system_clock::to_time_t(now);
    //   stringstream timestamp;
    //   timestamp << put_time(localtime(&time), "%m/%d/%Y %I:%M:%S %p");

    //   // Start file output
    //   string filename = format("memory_stamp_{}.txt", quantum_count);
    //   ofstream out(filename);
    //   if (!out) {
    //     cerr << "[error] Could not write to " << filename << "\n";
    //     return;
    //   }

    //   out << "Timestamp: " << timestamp.str() << "\n";
    //   out << "Number of processes in memory: " << mem.count_active() << "\n";
    //   out << "Total external fragmentation in KB: " << mem.external_fragmentation() / 1024 << "\n";
    //   out << "---end--- = 16384\n";

    //   // Print blocks from high to low (reverse order)
    //   for (auto it = blocks.rbegin(); it != blocks.rend(); ++it) {
    //     const auto& b = *it;
    //     const auto label = (b.pid == -1 ? "" : format("P{}", b.pid));
    //     const uint32_t start = b.start;
    //     const uint32_t end = b.start + b.size;

    //     // Print each 16-byte frame inside the block
    //     for (uint32_t addr = end; addr > start; addr -= 16) {
    //       out << addr << "\n";
    //       if (!label.empty()) out << label << "\n";
    //     }
    //   }

    //   out << "---start--- = 0\n";
    // }




  };
}
