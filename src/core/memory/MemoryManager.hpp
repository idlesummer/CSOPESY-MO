#pragma once
#include "core/common/imports/_all.hpp"
#include "MemoryManagerData.hpp"
#include "MemoryView.hpp"
#include "types.hpp"


class MemoryManager {
  public:

  MemoryManager():
    data (MemoryManagerData()) {}

  /**
   * @brief Initializes the memory system and sets up free frames and internal state.
   *
   * @param memory_capacity Total memory size in bytes.
   * @param page_size       Size of each page/frame in bytes.
   * @param is_preempted    Optional predicate function that returns true if a given PID is preempted.
   */
  void init(uint memory_capacity, uint page_size, func<bool(uint)> is_preempted=nullptr) {    
    data.page_size = page_size;
    data.frame_count = memory_capacity / page_size;
    data.memory = vec<uint>(memory_capacity, 0);

    // Store the preemption-check callback
    data.is_preempted = move(is_preempted);
  
    // Reset and repopulate the free frame list
    data.free_frames.clear(); 
    for (auto i=0u; i < data.frame_count; ++i)
      data.free_frames.push_back(i);

    // Clear all process page tables
    data.page_table_map.clear();
  }

  auto create_memory_view_for(uint pid, uint bytes_needed=64u) -> MemoryView {
    // Enforce minimum size of 64 bytes (symbol table)
    if (bytes_needed < 64u)
      bytes_needed = 64u;

    // Auto-allocate if not yet allocated
    if (!data.page_table_map.contains(pid))
      alloc(pid, bytes_needed);  // Auto-alloc with minimum default

    return MemoryView(pid, data, [this](uint pid, uint page_num) {
      return this->page_in(pid, page_num);
    });
  }

  /**
   * @brief Releases all memory used by a given process.
   *
   * Frees all physical frames occupied by the process's pages and
   * clears their corresponding page table and eviction queue entries.
   * This should be called when a process finishes execution or is terminated.
   */
  void release_all_frames_for(uint pid) {
    if (!data.page_table_map.contains(pid)) return;

    auto& page_table = data.page_table_map.at(pid);

    // Free any frames currently in use
    for (auto& page_num : page_table.pages()) {
      auto& page = page_table.get(page_num);
      if (page.is_loaded()) {
        data.free_frames.push_back(page.frame());
        page.frame_num = nullopt;
      }
    }

    // Remove from page table
    data.page_table_map.erase(pid);

    // Remove from eviction queue
    erase_if(data.equeue, [&](auto& entry) {
      auto [entry_pid, _] = entry;
      return entry_pid == pid;
    });
  }

  /**
 * @brief Renders a visual map of physical frames and their assigned virtual pages.
 * 
 * Each line shows:
 *   - Frame index
 *   - Owning process ID (if any)
 *   - Virtual page number (if mapped)
 *   - "free" if the frame is unused
 */
  auto render_layout() -> str {
    // Create reverse map: frame_num → (pid, page_num)
    auto frame_map = umap<uint, tup<uint, uint>>();

    for (auto& [pid, table] : data.page_table_map) {
      for (auto page_num : table.pages()) {
        auto& page = table.get(page_num);
        if (page.is_loaded()) {
          frame_map[page.frame()] = { pid, page_num };
        }
      }
    }

    // Render table
    auto out = osstream();
    out << "\n[Physical Memory Layout]\n";
    for (auto i = 0u; i < data.frame_count; ++i) {
      if (!frame_map.contains(i))
        out << format("[frame {:>2}] → free\n", i);
      else {
        auto [pid, page] = frame_map[i];
        out << format("[frame {:>2}] → pid={:<3} page={}\n", i, pid, page);
      }
    }

    return out.str();
  }

  // ------ Member variables ------
  MemoryManagerData data;
  
  // ------ Internal helpers ------
  private:

  auto alloc(uint pid, uint bytes_needed) -> bool {
    auto pages_needed = (bytes_needed + data.page_size - 1) / data.page_size;
    auto page_table = PageTable();

    for (auto page_num=0; page_num < pages_needed; ++page_num)
      page_table.add(page_num, PageEntry());

    data.page_table_map[pid] = move(page_table);

    // Try to preload as many pages as possible (no fault if not all succeed)
    for (auto page_num = 0u; page_num < pages_needed; ++page_num)
      page_in(pid, page_num);  // Ignore return value

    return true;
  }

  /** Fills the given frame in memory using the provided filler function. */
  void fill_frame(uint frame_num, func<uint(uint)> filler) {
    auto maddr = frame_num * data.page_size;
    for (auto i = 0u; i < data.page_size; ++i)
      data.memory[maddr + i] = filler(i);
  }

  /**
   * Loads the given virtual page into memory for a process.
   * May evict another page if no free frame is available (side effect).
   */
  auto page_in(uint pid, uint page_num) -> bool {
    // Try to get a free frame
    if (data.free_frames.empty())
      if (!page_out(pid))
        return false;   // Couldn't evict a page!
    
    // Retrieve free frame
    auto frame_num = data.free_frames.front();
    data.free_frames.pop_front();

    // Fetch page table and page metadata
    auto& table = data.page_table_map[pid];
    auto& page = table.get(page_num);
    
    // Assign physical frame to page
    page.frame_num = frame_num;
    data.equeue.push_back({ pid, page_num });

    // Fill frame with content from backing store or zero-fill
    auto key = make_key(pid, page_num);
    if (data.store.contains(key)) {
      auto bytes = move(data.store.at(key));
      data.store.erase(key);
      fill_frame(frame_num, [&](uint i) -> uint { return bytes[i]; });
      // cout << format("  [restore] pid={} page={} → frame={} ← restored from store\n", pid, page_num, frame_num);
    }
    else {
      fill_frame(frame_num, [](uint) -> uint { return 0; });
      // cout << format("  [zero]    pid={} page={} → frame={} ← filled with zeros\n", pid, page_num, frame_num);
    }

    return true;
  }

  /**
    * Evicts one loaded page from memory and returns its freed frame number.
    * This simulates basic page replacement when memory is full.
    * Also known as: "evict_page".
    */
  auto page_out(uint pid) -> bool {
    for (auto it = data.equeue.begin(); it != data.equeue.end(); ++it) {
      auto [evict_pid, page_num] = *it;

      // Evict only if:
      // - The page belongs to this process (self-eviction), OR
      // - The page belongs to a process that is currently preempted or inactive
      if (!(evict_pid == pid || (data.is_preempted && data.is_preempted(evict_pid))))
        continue;

      // Get page table and page entry
      auto& page_table = data.page_table_map.at(evict_pid);
      auto& page = page_table.get(page_num);

      if (!page.is_loaded())
        continue;  // Already unloaded, shouldn't happen but safe to skip

      // Get frame number and mark page as unloaded
      auto frame_num = page.frame();
      page.frame_num = nullopt;
      data.free_frames.push_back(frame_num);

      // Save to backing store
      auto maddr = frame_num * data.page_size;
      auto start = data.memory.begin() + maddr;
      auto end = start + data.page_size;
      data.store[make_key(pid, page_num)] = vec<uint>(start, end);

      // Remove from global queue
      data.equeue.erase(it);
      return true;
    }

    // No evictable pages found for this process
    return false;
  }

  /**
   * Computes a unique 64-bit key for a process-page pair.
   * Used as the backing store index for evicted pages.
   */
  auto make_key(uint pid, uint page_num) -> uint64 {
    return (cast<uint64>(pid) << 32) | page_num;
  }
};
