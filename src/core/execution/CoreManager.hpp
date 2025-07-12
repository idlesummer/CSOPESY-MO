#pragma once
#include "core/common/imports/_all.hpp"
#include "core/execution/Core.hpp"

namespace csopesy {

  /**
   * @brief Manages a pool of CPU cores in the scheduler.
   * 
   * This class provides access, iteration, and lifecycle
   * control (start/stop) for all managed `Core`s.
   */
  class CoreManager {
    using CorePtr = unique_ptr<Core>;    // Internal ownership via unique_ptr
    using CoreRefs = vector<ref<Core>>;  // For exposing multiple cores
    using Cores = vector<CorePtr>;       // Internal list of cores
    using list = vector<uint>;           // For returning PID lists
    
    Cores cores;

    public:

    /** Constructs and initializes a given number of cores */
    CoreManager(uint size=0) { resize(size); }

    /** Clears and reinitializes the core list with the specified number of cores */
    void resize(uint size) {
      cores.clear();
      cores.reserve(size);
      for (uint i=0; i < size; ++i) {
        cout << "[CoreManager::resize] Resizing cores.\n";
        cores.emplace_back(make_unique<Core>(i));
      }
    }

    // === Accessors ===

    /** @brief Returns number of cores. */
    uint size() const { return cores.size(); }

    /** @brief Access a specific core by index. */
    Core& get(uint i) { return *cores[i]; }

    /** @brief Returns the current CPU core utilization as a float [0.0, 1.0]. */
    float get_usage() const {
      if (cores.empty()) return 0.0f;
      uint busy = count_if(cores, [](auto& ptr) { return !ptr->is_idle(); });
      return cast<float>(busy) / cast<float>(cores.size());
    }

    /** Returns references to all cores */
    CoreRefs get_all() {
      auto all = CoreRefs();
      all.reserve(cores.size());

      for (auto& ptr: cores)
        all.push_back(ref(*ptr));
      return all;
    }

    /** Returns a list of references to all idle cores */
    CoreRefs get_idle() {
      auto idle = CoreRefs();
      idle.reserve(cores.size());

      for (auto& ptr: cores) {
        if (ptr->is_idle())
          idle.push_back(ref(*ptr));
      }
      return idle;
    }

    /** Returns a list of references to all busy (non-idle) cores */
    CoreRefs get_busy() {
      auto busy = CoreRefs();
      busy.reserve(cores.size());

      for (auto& ptr: cores) {
        if (ptr && !ptr->is_idle())
          busy.push_back(ref(*ptr));
      }
      return busy;
    }

    /** @brief Returns a list of IDs for all busy (non-idle) cores. */
    list get_busy_core_ids() {
      list ids;
      for (auto& ref: get_busy())
        ids.push_back(ref.get().id);
      return ids;
    }

    /** Returns a list of pids to all busy (non-idle) cores */
    list get_running_pids() {
      list pids;
      for (auto& ref: get_busy())
        pids.push_back(ref.get().get_job().data.id);
      return pids;
    }
  };
}
