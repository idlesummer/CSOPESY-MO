#pragma once
#include "core/common/imports/_all.hpp"
#include "types.hpp"

namespace csopesy {
  class ProcessMemory {
    using map = unordered_map<str, uint>;
    
  public:
    map symbol_table;

    /** Memory mutators */
    void set(str key, uint value) {
      symbol_table[move(key)] = value;
    }

    /** Memory accessors */
    uint get(const str& key) const {
      auto it = symbol_table.find(key);
      return it != symbol_table.end() ? it->second : 0;
    }
    
    /** Resolves a token as a literal or variable lookup */
    uint resolve(const str& token) const {
      return isdigit(token[0]) ? stoul(token) : get(token);
    }

    /** Exposes internal variable bindings */
    const auto& get_symbol_table() const {
      return symbol_table;
    }
  };
}
