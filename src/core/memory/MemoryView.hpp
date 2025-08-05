#pragma once
#include "core/common/imports/_all.hpp"
#include "MemoryManagerData.hpp"
#include "types.hpp"


class MemoryView {
  public:

  MemoryView(uint pid, MemoryManagerData& data, func<bool(uint, uint)> page_in):
    pid     (pid),        // Process ID owning this memory view
    data    (data),       // Reference to shared memory manager state (frames, tables, store, etc.)
    page_in (page_in) {}  // Page-in callback: loads (pid, page_num) into memory if not present

  /**
   * @brief Reads a 2-byte value from virtual memory.
   * @return (value, is_violation, is_page_fault)
   *         - value: the uint16 read from memory (or 0 if failed)
   *         - is_violation: true if address is invalid or unmapped
   *         - is_page_fault: true if a page fault occurred
   */
  auto read(uint vaddr) -> tup<int, bool, bool> {
    if (!has_pages_for(vaddr, 2))
      return {0, true, false}; // access violation

    auto m1 = maddr_of(vaddr);      
    auto m2 = maddr_of(vaddr + 1);
    if (!m1 || !m2)
      return {0, false, true}; // page fault

    auto lower_byte = data.memory[*m1];
    auto upper_byte = data.memory[*m2] << 8;
    return {lower_byte | upper_byte, false, false};
  }
    
  /**
   * @brief Writes a 2-byte value to virtual memory.
   * @return (is_violation, is_page_fault)
   *        - is_violation: true if address is invalid or unmapped
   *        - is_page_fault: true if a page fault occurred
   */
  auto write(uint vaddr, uint value) -> tup<bool, bool> {
    if (!has_pages_for(vaddr, 2))
      return {true, false}; // access violation

    auto m1 = maddr_of(vaddr);
    auto m2 = maddr_of(vaddr+1);
    if (!m1 || !m2)
      return {false, true}; // page fault

    data.memory[*m1] = value & 0xFF;
    data.memory[*m2] = (value >> 8) & 0xFF;
    return {false, false};
  }

  // ------ Member variables ------
  uint pid;
  MemoryManagerData& data;
  func<bool(uint,uint)> page_in;
  
  // ------ Internal Helpers ------
  private:

  /** @brief Checks whether the process has declared all pages needed for a virtual memory access. */
  auto has_pages_for(uint vaddr, uint num_bytes=1) -> bool {
    auto it = data.page_table_map.find(pid);
    if (it == data.page_table_map.end())
      return false;
    
    auto& page_table = it->second;
    auto start = vaddr / data.page_size;
    auto end = (vaddr + num_bytes - 1) / data.page_size;

    for (auto page_num=start; page_num <= end; ++page_num)
      if (!page_table.has_page(page_num))
        return false;
    return true;
  }

  /**
   * @brief Translates a virtual address to a physical memory address for the current process.
   * Triggers a page-in if the page is not loaded; returns nullopt on failure.
   * Returns the physical address (frame * page_size + offset) if successful.
   */
  // auto maddr_of(uint vaddr) -> opt<uint> {
  //   auto page_num = vaddr / data.page_size;
  //   auto offset   = vaddr % data.page_size;
  //   auto& page_table = data.page_table_map.at(pid);
  //   auto& page = page_table.get(page_num);

  //   if (!page.is_loaded() && !page_in(pid, page_num)) // Note: page_in method has side effects
  //     return nullopt;
  //   return page.frame() * data.page_size + offset;
  // }

  auto maddr_of(uint vaddr) -> opt<uint> {
    auto page_num = vaddr / data.page_size;
    auto offset   = vaddr % data.page_size;
    auto& page_table = data.page_table_map.at(pid);
    auto& page = page_table.get(page_num);

    if (!page.is_loaded()) {
      // attempt to page in
      bool success = page_in(pid, page_num);
      if (!success) return nullopt;

      // if now loaded after success, count it
      if (page.is_loaded()) ++data.num_paged_in;
    }

    return page.frame() * data.page_size + offset;
  }
};
