#pragma once
#include "core/common/imports/_all.hpp"

using uint = unsigned int; 

namespace csopesy {

  class MemoryManager {
    static constexpr uint TOTAL_MEM = 16384;    // Total memory in bytes
    static constexpr uint PROC_MEM  = 4096;     // Memory per process in bytes

    struct Block {
      uint base;  // Inclusive
      uint limit; // Exclusive
      uint pid;
    };

    std::vector<Block> allocations;

    std::string current_time_string() {
      auto now = std::chrono::system_clock::now();
      std::time_t t = std::chrono::system_clock::to_time_t(now);
      std::tm tm = *std::localtime(&t);
      char buf[64];
      std::strftime(buf, sizeof(buf), "%m/%d/%Y %I:%M:%S%p", &tm);
      return std::string(buf);
    }

  public:
    /** Attempt to allocate memory for a process using first-fit. */
    bool allocate(uint pid) {
      if (has(pid)) return true;
      std::sort(allocations.begin(), allocations.end(), [](auto& a, auto& b) {
        return a.base < b.base;
      });
      uint prev_end = 0;
      for (const auto& block : allocations) {
        if (block.base - prev_end >= PROC_MEM) {
          allocations.push_back({prev_end, prev_end + PROC_MEM, pid});
          return true;
        }
        prev_end = block.limit;
      }
      if (TOTAL_MEM - prev_end >= PROC_MEM) {
        allocations.push_back({prev_end, prev_end + PROC_MEM, pid});
        return true;
      }
      return false;
    }

    /** Free memory occupied by the given process. */
    void release(uint pid) {
      std::erase_if(allocations, [&](const Block& b) {
        return b.pid == pid;
      });
    }

    /** Return true if the process already has an allocation. */
    bool has(uint pid) const {
      for (const auto& b : allocations)
        if (b.pid == pid)
          return true;
      return false;
    }

    /** Write a snapshot of memory state to a text file. */
    void snapshot(uint cycle) {
      std::string fname = std::format("memory_stamp_{}.txt", cycle);
      std::ofstream out(fname);
      if (!out) return;

      out << std::format("Timestamp: ({})\n", current_time_string());
      out << std::format("Number of processes in memory: {}\n", allocations.size());

      std::sort(allocations.begin(), allocations.end(), [](auto& a, auto& b) {
        return a.base < b.base;
      });

      uint frag = 0, prev_end = 0;
      for (const auto& block : allocations) {
        if (block.base > prev_end)
          frag += block.base - prev_end;
        prev_end = block.limit;
      }
      if (prev_end < TOTAL_MEM)
        frag += TOTAL_MEM - prev_end;

      out << std::format("Total external fragmentation in KB: {}\n", frag);

      out << "\n----end---- = " << TOTAL_MEM << "\n";

      // Print blocks in reverse order (top-down)
      std::vector<Block> reverse = allocations;
      std::sort(reverse.begin(), reverse.end(), [](auto& a, auto& b) {
        return a.limit > b.limit;
      });

      prev_end = TOTAL_MEM;
      for (const auto& block : reverse) {
        if (prev_end > block.limit)
          out << prev_end << "\n";
        out << "P" << block.pid << "\n";
        out << block.base + PROC_MEM << "\n";
        prev_end = block.base;
      }
      if (prev_end > 0)
        out << prev_end << "\n----start---- = 0\n";
    }

    /** Clear all memory (useful for resets). */
    void reset() {
      allocations.clear();
    }
  };
}
