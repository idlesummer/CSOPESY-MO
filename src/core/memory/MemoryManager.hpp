#include "core/common/imports/_all.hpp"

struct MemoryBlock {
  uint32_t start;
  uint32_t size;
  bool allocated;
  uint32_t pid;
};

//** To decide whether and where a process can fit in memory — and to track and update memory usage as processes run and finish. */
class MemoryManager {

public:
    MemoryManager() {
    blocks.push_back({0, total_size, false, 0});
  }

  // Allocate memory using first-fit
  bool allocate(uint32_t pid) {
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
      if (!it->allocated && it->size >= process_size) {
        uint32_t remaining = it->size - process_size;
        uint32_t alloc_start = it->start;

        *it = {alloc_start, process_size, true, pid};

        if (remaining > 0)
          blocks.insert(it + 1, {alloc_start + process_size, remaining, false, 0});

        return true;
      }
    }
    return false;
  }

  // Release memory for a process
  void release(uint32_t pid) {
    for (auto& block : blocks) {
      if (block.allocated && block.pid == pid) {
        block.allocated = false;
        block.pid = 0;
        break;
      }
    }
    merge_free_blocks();
  }

  // Output memory snapshot to file
  void dump_snapshot(uint32_t quantum_index) const {
    std::ostringstream filename;
    filename << "memory_stamp_" << std::setw(2) << std::setfill('0') << quantum_index << ".txt";
    std::ofstream out(filename.str());

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    out << "Timestamp: (" << std::put_time(std::localtime(&now), "%m/%d/%Y %I:%M:%S%p") << ")\n";
    out << "Number of processes in memory: " << get_process_count() << '\n';
    out << "Total external fragmentation in KB: " << get_external_fragmentation() << '\n';
    out << "-----end----- = " << total_size << '\n';

    uint32_t cursor = total_size;
    for (auto it = blocks.rbegin(); it != blocks.rend(); ++it) {
      const auto& b = *it;
      cursor -= b.size;
      if (b.allocated) {
        out << cursor + b.size << '\n';
        out << "P" << b.pid << '\n';
        out << cursor << '\n';
      }
    }

    out << "-----start----- = 0\n";
  }

  // Count number of currently allocated processes
  uint32_t get_process_count() const {
    uint32_t count = 0;
    for (const auto& b : blocks)
      if (b.allocated)
        ++count;
    return count;
  }

  // Sum of all unallocated holes smaller than process size
  uint32_t get_external_fragmentation() const {
    uint32_t total = 0;
    for (const auto& b : blocks)
      if (!b.allocated && b.size < process_size)
        total += b.size;
    return total / 1024; // return in KB
  }


private:    

    void merge_free_blocks() {
    for (auto it = blocks.begin(); it != blocks.end(); ) {
      if (!it->allocated) {
        auto next = it + 1;
        while (next != blocks.end() && !next->allocated) {
          it->size += next->size;
          next = blocks.erase(next);
        }
      }
      ++it;
    }
  }
    uint32_t total_size = 16384;
    uint32_t process_size = 4096;
    uint32_t frame_size = 16;

    std::vector<MemoryBlock> blocks;
};