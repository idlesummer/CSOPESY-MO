#pragma once
#include "core/common/imports/_all.hpp"
#include "core/instruction/types.hpp"

namespace csopesy {
  enum class State {
    Ready,
    Sleeping,
    Finished,
  };

  /** A loop frame from a FOR-like instruction. */
  struct LoopFrame {
    using Block  = ref<const vector<Instruction>>;
    using Stack  = vector<LoopFrame>;

    uint ip = 0;     ///< Current instruction index within the block
    uint count;      ///< Number of remaining iterations
    Block block;     ///< Reference to the loop's instruction block

    /** Constructs a loop frame from a FOR-like instruction. */
    explicit LoopFrame(const Instruction& inst):
      ip(0),
      count(cast_uint(inst.args[0])),
      block(cref(inst.block)) {}

    /** Returns true if the instruction pointer has reached the end of the block. */
    bool end_of_block() const {
      return ip >= block.get().size();
    }

    /** Returns true if the loop has completed all iterations. */
    bool should_exit() const {
      return count == 0;
    }

    /** Advances to the next iteration by decrementing count and resetting the instruction pointer. */
    void next_iteration() {
      --count;
      ip = 0;
    }
  };
}
