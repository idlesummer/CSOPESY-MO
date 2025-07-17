#pragma once
#include "core/common/imports/_all.hpp"
#include "core/execution/Core.hpp"


/**
 * @brief Manages the pool of Core components for the scheduler system.
 * 
 * Responsibilities:
 * - Owns and maintains all simulated CPU cores as `Core` objects.
 * - Provides safe and filtered access to all, busy, or idle cores.
 * - Handles core lifecycle: creation, reset, and safe iteration.
 * 
 * Key Behaviors:
 * - `resize(size)`: Replaces all cores with a new set of N fresh cores.
 * - `get_busy()` / `get_idle()`: Filters out cores by status for use in scheduling logic.
 * - `get_usage()`: Returns a float utilization ratio (busy / total cores).
 * - `get_running_pids()` and `get_busy_core_ids()`: Extract diagnostic metadata from active cores.
 * 
 * Design Notes:
 * - Cores are stored as `unique_ptr<Core>` to ensure clean ownership and destruction.
 * - Always use reference-returning accessors (`CoreRefs`) instead of raw pointer access.
 * - Guards against null pointers even though `resize()` always populates all slots.
 * - Scheduler is the only component expected to use this class directly.
 */
class CoreManager {
  using CoreRefs = vector<ref<Core>>; // For exposing multiple cores as return values
  using CorePtr = unique_ptr<Core>;   // Internal ownership via unique_ptr
  using Cores = vector<CorePtr>;      // Internal list of cores
  using list = vector<uint>;          // For returning PID lists
  using bool_func = function<bool(CorePtr&)>; // For private helper
  using uint_func = function<uint(Core&)>;    // For private helper
  
  Cores cores = {};

  public:

  /** @brief Clears and reinitializes the core list with the specified number of cores */
  void resize(uint size) {
    cores.clear();                    // Destroys all existing cores
    cores.reserve(size);              // Optional: preallocate to avoid reallocations
    for (uint i=0; i < size; ++i) {
      cores.emplace_back(make_unique<Core>(i));
    }
  }

  // === Accessors ===

  /** @brief Returns number of cores. */
  uint size() const { return cores.size(); }
  
  /** @brief @brief Returns the current CPU core utilization as a float [0.0, 1.0]. */
  float get_usage() const {
    if (cores.empty()) return 0.0f;
    return cast<float>(get_busy_size()) / cast<float>(cores.size());
  }

  /** @brief @brief Access a specific core by index. */
  Core& get(uint i) { return *cores.at(i); }

  /** @brief Returns references to all cores. */
  CoreRefs get_all() { return filter_cores([](auto& ptr) { return true; }); }

  /** @brief Returns a list of references to all idle cores. */
  CoreRefs get_idle() { return filter_cores([](auto& ptr) { return ptr->is_idle(); }); }

  /** @brief Returns a list of references to all busy (non-idle) cores. */
  CoreRefs get_busy() { return filter_cores([](auto& ptr) { return !ptr->is_idle(); }); }
  
  /** @brief Returns a list of references to all busy (non-idle) and releasable  cores. */
  CoreRefs get_releasable() { return filter_cores([](auto& ptr) { return !ptr->is_idle() && ptr->can_release; }); }

  /** @brief @brief Returns a list of IDs for all busy (non-idle) cores. */
  list get_busy_core_ids() { return extract_ids([](Core& core) { return core.id; }); }

  /** @brief Returns a list of pids to all busy (non-idle) cores. */
  list get_running_pids() { return extract_ids([](Core& core) { return core.get_job().data.id; }); }

  // ========================
  // === Private Members ====
  // ========================
  private:

  // === Helper Methods === 

  /** @brief Returns the number of currently busy (non-idle) cores. */
  uint get_busy_size() const {
    return count_if(cores, [](auto& ptr) { return ptr != nullptr && !ptr->is_idle(); });
  }

  /** @brief Returns references to cores that satisfy the given filter condition. */
  CoreRefs filter_cores(bool_func predicate) {
    auto result = CoreRefs();       // Create a list of core reference wrappers
    result.reserve(cores.size());   // Optional: preallocate to avoid reallocations

    for (auto& ptr: cores)
      if (ptr != nullptr && predicate(ptr)) // Only include non-null cores that pass the filter
        result.push_back(ref(*ptr));        // Store a reference wrapper of the matching core
    return result;
  }

  /** @brief Extracts a list of uint values from all busy cores using the given accessor. */
  list extract_ids(uint_func getter) {
    auto busy = get_busy();         // Store once to reuse
    list result;
    result.reserve(busy.size());    // Reserve exactly what we need

    for (auto& ref: busy)
      result.push_back(getter(ref.get()));
    return result;
  }
};
