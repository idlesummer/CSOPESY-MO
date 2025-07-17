#pragma once
#include "core/common/imports/_all.hpp"
#include "ProcessProgram.hpp"


/**
 * @brief Tracks the execution state of a process.
 * 
 * Supports transitions between Ready, Sleeping, and Finished states,
 * and stores the number of ticks remaining for a sleeping process.
 */
class ProcessControl {
  public:

  uint sleep_ticks = 0;
  
  // === Methods ===
  
  /** @brief Returns true if the process is currently sleeping. */
  bool sleeping() const { return sleep_ticks > 0; }    

  /** @brief Puts the process to sleep for a number of ticks. */
  void sleep_for(uint ticks) { sleep_ticks = ticks; }

  /** @brief Advances sleep state by one tick, if sleeping. */
  void tick() { if (sleep_ticks > 0) --sleep_ticks; }
};
