#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <thread>

namespace csopesy {

  // === Atomic Types ===
  using std::atomic;
  using std::atomic_bool;
  using std::atomic_int;
  using std::atomic_uint;

  // === Locking & Threading Aliases ===
  using std::lock_guard;
  using std::mutex;
  using std::shared_lock;
  using std::shared_mutex;
  using std::this_thread::sleep_for;
  using std::unique_lock;
  using Thread = std::thread;
  using convar = std::condition_variable;
  
  // === Scoped Lock Execution Context ===
  struct Context {
    /**
     * @brief Acquires an exclusive lock using std::mutex.
     * @param mtx The mutex to lock.
     * @param fn The function to execute within the lock.
     */
    template <typename Func>
    void lock(mutex& mtx, Func&& fn) const {
      auto guard = lock_guard(mtx);
      std::forward<Func>(fn)();
    }

    /**
     * @brief Acquires an exclusive lock using std::mutex and passes the lock to the function.
     * @param mtx The mutex to lock.
     * @param fn The function to execute with the lock passed as argument.
     */
    template <typename Func>
    void unique(mutex& mtx, Func&& fn) const {
      auto lock = unique_lock(mtx);
      std::forward<Func>(fn)(lock);
    }
  };

  // === Scoped Context Instance ===
  inline constexpr Context with;
}
