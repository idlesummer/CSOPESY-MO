#pragma once
#include "core/common/imports/_all.hpp"


/** @brief Represents the metadata of a virtual page (not the actual content). */
class PageEntry {
  public:

  PageEntry():
  frame_num (nullopt) {} // The physical frame index (nullopt if not loaded)
  
  /** @brief Returns true if this page is loaded in a physical frame. */
  auto is_loaded() -> bool { return frame_num.has_value(); }
  
  /** @brief Returns the physical frame number. Throws if not loaded. */
  auto frame() -> uint { return frame_num.value(); }
  
  // ------ Member variables ------
  opt<uint> frame_num;
};


/**
 * @brief Represents the page table for a process.
 * Maps virtual page numbers to their PageEntry metadata.
 */
class PageTable {
  public:

  PageTable(): 
    entries (umap<uint,PageEntry>()) {}  // Virtual page number: metadata

  /** @brief Returns true if this table tracks the given virtual page. */
  auto has_page(uint page_num) -> bool { return entries.contains(page_num); }

  /** @brief Returns a reference to the PageEntry for the given page. */
  auto get(int page_num) -> PageEntry& { return entries.at(page_num); }

  /** @brief Adds a PageEntry to the table for the given page number. */
  void add(uint page_num, PageEntry page) { entries[page_num] = page; }

  /** @brief Returns a view of all stored virtual page numbers. */
  auto pages() -> vec<uint> {
    auto keys = vec<uint>();
    keys.reserve(entries.size());

    for (auto& [key, _]: entries)
      keys.push_back(key);
    return keys;
  }

  // ------ Member variables ------
  umap<uint,PageEntry> entries;
};
