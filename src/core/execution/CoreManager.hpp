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
 * - vec<uptr<Core>> are stored as `uptr<Core>` to ensure clean ownership and destruction.
 * - Always use reference-returning accessors (`vec<ref<Core>>`) instead of raw pointer access.
 * - Guards against null pointers even though `resize()` always populates all slots.
 * - Scheduler is the only component expected to use this class directly.
 */
class CoreManager {
  using bool_func = function<bool(uptr<Core>&)>; // For private helper
  using uint_func = function<uint(Core&)>;    // For private helper
  
  public:

  CoreManager():
    cores (vec<uptr<Core>>()) {}

  /** @brief Clears and reinitializes the core vec<uint> with the specified number of cores */
  void resize(uint size) {
    cores.clear();                    // Destroys all existing cores
    cores.reserve(size);              // Optional: preallocate to avoid reallocations
    for (uint i=0; i < size; ++i)
      cores.emplace_back(make_unique<Core>(i));
  }

  // === Accessors ===

  /** @brief Returns number of cores. */
  auto size() -> uint const { return cores.size(); }
  
  /** @brief @brief Returns the current CPU core utilization as a float [0.0, 1.0]. */
  auto get_usage() -> float const {
    if (cores.empty()) return 0.0f;
    return cast<float>(get_busy_size()) / cast<float>(cores.size());
  }

  /** @brief @brief Access a specific core by index. */
  auto get(uint i) -> Core& { return *cores.at(i); }

  /** @brief Returns references to all cores. */
  auto get_all() -> vec<ref<Core>> { return filter_cores([](auto& ptr) { return true; }); }

  /** @brief Returns a vec<uint> of references to all idle cores. */
  auto get_idle() -> vec<ref<Core>> { return filter_cores([](auto& ptr) { return ptr->is_idle(); }); }

  /** @brief Returns a vec<uint> of references to all busy (non-idle) cores. */
  auto get_busy() -> vec<ref<Core>> { return filter_cores([](auto& ptr) { return !ptr->is_idle(); }); }
  
  /** @brief Returns a vec<uint> of references to all busy (non-idle) and releasable  cores. */
  auto get_releasable() -> vec<ref<Core>> { return filter_cores([](auto& ptr) { return !ptr->is_idle() && ptr->can_release; }); }

  /** @brief @brief Returns a vec<uint> of IDs for all busy (non-idle) cores. */
  auto get_busy_core_ids() -> vec<uint> { return extract_ids([](Core& core) { return core.id; }); }

  /** @brief Returns a vec<uint> of pids to all busy (non-idle) cores. */
  auto get_running_pids() -> vec<uint> { return extract_ids([](Core& core) { return core.get_job().data.id; }); }

  // ------ Internal Logic ------

  private:

  // ------ Member variables ------
  vec<uptr<Core>> cores;

  // ------ Helper Methods ------

  /** @brief Returns the number of currently busy (non-idle) cores. */
  auto get_busy_size() -> uint const {
    return count_if(cores, [](auto& ptr) { return ptr != nullptr && !ptr->is_idle(); });
  }

  /** @brief Returns references to cores that satisfy the given filter condition. */
  vec<ref<Core>> filter_cores(bool_func predicate) {
    auto result = vec<ref<Core>>();       // Create a vec<uint> of core reference wrappers
    result.reserve(cores.size());   // Optional: preallocate to avoid reallocations

    for (auto& ptr: cores)
      if (ptr != nullptr && predicate(ptr)) // Only include non-null cores that pass the filter
        result.push_back(ref(*ptr));        // Store a reference wrapper of the matching core
    return result;
  }

  /** @brief Extracts a vec<uint> of uint values from all busy cores using the given accessor. */
  auto extract_ids(uint_func getter) -> vec<uint> {
    auto busy = get_busy();         // Store once to reuse
    auto result = vec<uint>();
    result.reserve(busy.size());    // Reserve exactly what we need

    for (auto& ref: busy)
      result.push_back(getter(ref.get()));
    return result;
  }
};
