#pragma once
#include "core/common/imports/_all.hpp"
#include "types.hpp"


class MemoryManagerData {
  public:

  MemoryManagerData():
    page_size      (0u),    
    frame_count    (0u),
    memory         (vec<uint>()),
    free_frames    (deque<uint>()),
    page_table_map (umap<uint,PageTable>()),
    equeue         (deque<tup<uint, uint>>()),
    store          (umap<uint64,vec<uint>>()),
    is_preempted   (nullptr) {}

  // ------ Member variables ------
  uint page_size;
  uint frame_count;
  vec<uint> memory;
  deque<uint> free_frames;
  umap<uint,PageTable> page_table_map;
  deque<tup<uint, uint>> equeue;
  umap<uint64,vec<uint>> store;
  func<bool(uint)> is_preempted;
};
