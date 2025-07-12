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
    using CorePtr = unique_ptr<Core>;   // We need pointers since we can't move or copy threads
    using Cores = vector<CorePtr>;
    using CoreRefs = vector<ref<Core>>;
    
    Cores cores;

    public:

    /** Constructs and initializes a given number of cores. */
    CoreManager(uint size=0) { resize(size); }

    /** Clears and reinitializes the core list with the specified number of cores. */
    void resize(uint size) {
      cores.clear();
      cores.reserve(size);
      for (uint i = 0; i < size; ++i)
        cores.emplace_back(make_unique<Core>(i));
    }

    // === Accessors ===

    uint size() const { return cores.size(); }
    Core& get(uint i) { return *cores[i]; }
    const Core& get(uint i) const { return *cores[i]; }

    /** Returns references to all cores. */
    CoreRefs get_all() {
      auto all = CoreRefs();
      all.reserve(cores.size());

      for (auto& ptr: cores)
        all.push_back(ref(*ptr));
      return all;
    }

    /** Returns a list of references to all idle cores. */
    CoreRefs get_idle() {
      auto idle = CoreRefs();
      idle.reserve(cores.size());

      for (auto& ptr: cores) {
        if (ptr->is_idle())
          idle.push_back(ref(*ptr));
      }
      return idle;
    }

    /** Returns a list of references to all busy (non-idle) cores. */
    CoreRefs get_busy() {
      auto busy = CoreRefs();
      busy.reserve(cores.size());

      for (auto& ptr: cores) {
        if (!ptr->is_idle())
          busy.push_back(ref(*ptr));
      }
      return busy;
    }
  };
}
