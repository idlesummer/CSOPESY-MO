#pragma once
#include "core/common/imports/_all.hpp"
#include "MemoryManagerData.hpp"
#include "MemoryView.hpp"
#include "types.hpp"


class MemoryManager {
  public:

  MemoryManager():
    data (MemoryManagerData()) {}

  void init(uint capacity, uint page_size) {    
    data.page_size = page_size;
    data.frame_count = capacity / page_size;
    data.memory = vec<uint>(capacity, 0);
  
    // Reset and repopulate the free frame list
    data.free_frames.clear(); 
    for (auto i=0u; i < data.frame_count; ++i)
      data.free_frames.push_back(i);

    // Clear all process page tables
    data.page_table_map.clear();
  }

  auto alloc(uint pid, uint bytes_needed) -> bool {
    auto pages_needed = (bytes_needed + data.page_size - 1) / data.page_size;
    auto page_table = PageTable();

    for (auto page_num=0; page_num < pages_needed; ++page_num)
      page_table.add(page_num, PageEntry());

    data.page_table_map[pid] = move(page_table);
    return true;
  }

  auto memory_view_of(uint pid) -> MemoryView {
    return MemoryView(pid, data, [this](uint pid, uint page_num) {
      return this->page_in(pid, page_num);
    });
  }

  // ------ Member variables ------
  MemoryManagerData data;
  
  // ------ Internal helpers ------
  private:

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
      if (!page_out())
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
      cout << format("  [restore] pid={} page={} → frame={} ← restored from store\n", pid, page_num, frame_num);
    }
    else {
      fill_frame(frame_num, [](uint) -> uint { return 0; });
      cout << format("  [zero]    pid={} page={} → frame={} ← filled with zeros\n", pid, page_num, frame_num);
    }

    return true;
  }

  /**
    * Evicts one loaded page from memory and returns its freed frame number.
    * This simulates basic page replacement when memory is full.
    * Also known as: "evict_page".
    */
  auto page_out() -> bool {
    while (!data.equeue.empty()) {
      auto [pid, page_num] = data.equeue.front();
      data.equeue.pop_front();   

      auto& table = data.page_table_map[pid];
      auto& page = table.get(page_num);

      auto frame_num = page.frame();
      page.frame_num = nullopt;
      data.free_frames.push_back(frame_num);

      auto maddr = frame_num * data.page_size;
      auto start = data.memory.begin() + maddr;
      auto end = start + data.page_size;
      auto bytes_out = vec<uint>(start, end);
      data.store[make_key(pid, page_num)] = move(bytes_out);

      cout << format("  [evict]   pid={} page={} → freed frame={} → saved to store\n",
                     pid, page_num, frame_num);

      return true;  // Evicted a page to backing store!
    }

    cout << "  [evict]   page_out FAILED → no pages available to evict\n";
    return false;   // Nothing to evict
  }

  /**
   * Computes a unique 64-bit key for a process-page pair.
   * Used as the backing store index for evicted pages.
   */
  auto make_key(uint pid, uint page_num) -> uint64 {
    return (cast<uint64>(pid) << 32) | page_num;
  }
};
