#pragma once
#include "core/common/imports/_all.hpp"
#include "types.hpp"

namespace csopesy {
  class ProcessState {
  public:
    State state = State::Ready;
    uint sleep_ticks = 0;

    void sleep_for(uint ticks) {
      state = State::Sleeping;
      sleep_ticks = ticks;
    }

    bool is_sleeping() const {
      return state == State::Sleeping;
    }

    bool is_finished() const {
      return state == State::Finished;
    }

    void set_ready() {
      state = State::Ready;
    }

    void set_finished() {
      state = State::Finished;
    }
  };
}
