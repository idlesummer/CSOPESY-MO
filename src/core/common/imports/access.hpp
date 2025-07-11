#pragma once
#include <mutex>

namespace csopesy {

  /**
   * @brief Global mutex protecting all SchedulerData access.
   * 
   * This allows you to leave SchedulerData itself lock-free and instead
   * synchronize externally using a simple access(...) wrapper.
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
  decltype(auto) access(Func&& fn) {
    auto lock = std::lock_guard(mtx);
    return std::forward<Func>(fn)();
  }
}
