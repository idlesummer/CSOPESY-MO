#pragma once
#include "core/common/imports/_all.hpp"

namespace csopesy {
   struct Block {
    uint32_t start;
    uint32_t size;
    int pid = -1; // -1 = free
  };

  class MemoryManager {
    std::vector<Block> blocks;
    static constexpr uint32_t max_mem   = 16'384;
    static constexpr uint32_t proc_size = 4'096;

  public:
    MemoryManager() {
      blocks.push_back({0, max_mem, -1});
    }

    bool allocate(int pid) {
      for (size_t i = 0; i < blocks.size(); ++i) {
        auto& b = blocks[i];
        if (b.pid == -1 && b.size >= proc_size) {
          if (b.size > proc_size) {
            blocks.insert(blocks.begin() + i + 1, {
              b.start + proc_size,
              b.size - proc_size,
              -1
            });
          }
          b.size = proc_size;
          b.pid  = pid;
          return true;
        }
      }
      return false;
    }

    void deallocate(int pid) {
      for (auto& b : blocks) {
        if (b.pid == pid) {
          b.pid = -1;
          break;
        }
      }
      merge();
    }

    void merge() {
      for (size_t i = 0; i + 1 < blocks.size(); ) {
        if (blocks[i].pid == -1 && blocks[i + 1].pid == -1) {
          blocks[i].size += blocks[i + 1].size;
          blocks.erase(blocks.begin() + i + 1);
        } else {
          ++i;
        }
      }
    }

    uint32_t external_fragmentation() const {
      uint32_t total = 0;
      for (auto& b : blocks)
        if (b.pid == -1 && b.size < proc_size)
          total += b.size;
      return total;
    }

    int count_active() const {
      int count = 0;
      for (auto& b : blocks)
        if (b.pid != -1)
          ++count;
      return count;
    }

    const std::vector<Block>& get_blocks() const {
      return blocks;
    }
  };
}