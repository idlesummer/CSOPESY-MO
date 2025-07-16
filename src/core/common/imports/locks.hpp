#pragma once
#include <mutex>

namespace csopesy {

  /**
   * @brief Global mutex protecting all SchedulerData access.
   * 
   * This allows you to leave SchedulerData itself lock-free and instead
   * synchronize externally using a simple with_locked(...) wrapper.
   */
  inline std::mutex mtx;

  /**
   * @brief Executes a function with exclusive access to scheduler state.
   * 
   * This acquires a scoped lock on the global mutex, executes the
   * function, and returns its result. Automatically unlocks at scope end.
   * 
   * @tparam Func A callable type (lambda, functor, etc.)
   * @param fn The function to execute under lock
   * @return The result of the provided function (supports references and values)
   */
  template <typename Func>
  decltype(auto) with_locked(Func&& fn) {
    auto guard = std::lock_guard(mtx);
    return std::forward<Func>(fn)();
  }

  /**
   * @brief Temporarily releases the global lock to execute a function without holding it.
   * 
   * This is meant to be used **inside a with_locked section**, where the lock is
   * manually unlocked for the duration of this function and re-locked afterward.
   */
  template <typename Func>
  decltype(auto) with_unlocked(Func&& fn) {
    mtx.unlock();
    try {
      std::forward<Func>(fn)(); // Optional intermediate
      mtx.lock();  // re-lock before returning
    } catch (...) {
      mtx.lock();  // re-lock even if an exception is thrown
      throw;
    }
  }
}