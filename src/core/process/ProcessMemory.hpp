#pragma once
#include "core/common/imports/_all.hpp"
#include "core/memory/MemoryView.hpp"
#include "types.hpp"


class ProcessMemory { 
  public:
  
  ProcessMemory(MemoryView& view):
    symbol_table (umap<str,uint>()),  // TODO: docs
    view         (view) {}            // TODO: docs

  /** Memory mutators */
  void set(str key, uint value) {
    symbol_table[move(key)] = value;
  }
  
  /** Memory accessors */
  auto get(const str& key) const -> uint {
    auto it = symbol_table.find(key);
    return it != symbol_table.end() ? it->second : 0;
  }
  
  /** Resolves a token as a literal or variable lookup */
  auto resolve(const str& token) const -> uint {
    return isdigit(token[0]) ? stoul(token) : get(token);
  }
  
  /** Exposes internal variable bindings */
  auto get_symbol_table() -> umap<str,uint>& {
    return symbol_table;
  }

  // ------ Instance variables ------
  umap<str,uint> symbol_table;
  MemoryView& view;
};
