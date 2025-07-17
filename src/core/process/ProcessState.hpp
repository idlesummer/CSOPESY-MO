#pragma once
#include "core/common/imports/_all.hpp"
#include "types.hpp"


/**
 * @brief Tracks the execution state of a process.
 * 
 * Supports transitions between Ready, Sleeping, and Finished states,
 * and stores the number of ticks remaining for a sleeping process.
 */
class ProcessState {
  uint sleep_ticks = 0;
  
public:
  State value = State::Ready;

  // === State Queries ===

  /** @brief Returns true if the process is currently sleeping. */
  bool sleeping() const { return value == State::Sleeping; }
  
  /** @brief Returns true if the process has finished execution. */
  bool finished() const { return value == State::Finished; }
  
  // === State Transitions ===

  /** @brief Sets the process state to Ready. */
  void ready() { value = State::Ready; }

  /** @brief Sets the process state to Finished. */
  void finish() { value = State::Finished; }

  /** @brief Puts the process to sleep for a number of ticks. */
  void sleep_for(uint ticks) {
    value = State::Sleeping;
    sleep_ticks = ticks;
  }
};
