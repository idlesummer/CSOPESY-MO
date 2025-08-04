#pragma once
#include "core/common/imports/_all.hpp"
#include "core/memory/MemoryView.hpp"
#include "types.hpp"

/**
 * @brief Simulates per-process variable memory with virtual paging.
 * 
 * This class acts as the variable storage system for a process.
 * It maps variable names (like `x`, `y`) to virtual addresses,
 * and delegates all memory access to an underlying `MemoryView`
 * which supports paging, page faults, and memory protection.
 * 
 * It simulates a symbol table of up to 32 variables (64 bytes total),
 * where each variable takes up 2 bytes (16-bit values).
 * 
 * @note Use `set()` and `get()` to safely write and read memory.
 *       Use `resolve()` to decode literals or variables.
 */
class ProcessMemory {
  public:

  /** Constructs a process memory system with a backing memory view. */
  ProcessMemory(MemoryView view):
    symbol_table(umap<str,uint>()), // Maps variable names to virtual addresses
    virtual_memory (view),          // Reference to the process's virtual memory (MemoryView)
    next_addr      (0u),            // Next available address for a new variable (2-byte aligned)
    symbol_limit   (32u) {}         // Maximum number of variables allowed (2 bytes each)

  /**
   * @brief Writes a 2-byte (16-bit) value to a declared or new variable.
   * 
   * If the variable already exists, it overwrites its value in memory.
   * If it doesn't exist and the symbol table has space, it declares it.
   * 
   * @param var   The variable name (e.g. "x", "y")
   * @param value The 16-bit value to store
   * @return Tuple:
   *         - is_violation (bool): true if any memory access violation occurred
   *         - is_page_fault (bool): true if a page fault occurred
   *         - is_symbol_limit (bool): true if variable couldn't be declared (table full)
   */
  auto set(const str& var, uint value) -> tup<bool, bool, bool> {
    if (symbol_table.contains(var)) {
      auto addr = symbol_table[var];
      auto [is_violation, is_page_fault] = virtual_memory.write(addr, value);
      return {is_violation, is_page_fault, false};
    }

    if (symbol_table.size() < symbol_limit) {
      auto addr = next_addr;
      symbol_table[var] = addr;
      next_addr += 2;
      auto [is_violation, is_page_fault] = virtual_memory.write(addr, value);
      return {is_violation, is_page_fault, false};
    }

    // Could not declare — symbol table full
    return {true, false, true};
  }

  /**
   * @brief Reads a 2-byte (16-bit) value from a declared variable.
   * 
   * @param var The variable name to look up
   * @return Tuple:
   *         - value (uint): Value stored (or 0 if undeclared)
   *         - is_violation (bool): true if variable not declared
   *         - is_page_fault (bool): true if page fault occurred
   *         - is_undeclared (bool): true if variable is undeclared
   */
  auto get(const str& var) -> tup<uint,bool,bool,bool> {
    if (!symbol_table.contains(var))
      return {0, true, false, true}; // undeclared = true

    auto addr = symbol_table[var];
    auto [value, is_violation, is_page_fault] = virtual_memory.read(addr);
    return {value, is_violation, is_page_fault, false}; // undeclared = false
  }

  /**
   * @brief Resolves a token as either a numeric literal or a variable lookup.
   * 
   * If the token is numeric, returns its value directly.
   * Otherwise, looks it up as a variable via `get()`.
   * 
   * @param token The input string (e.g. "42", "x")
   * @return Tuple:
   *         - value (uint): Resolved value
   *         - violation (bool): true if undeclared variable
   *         - page_fault (bool): true if page fault occurred
   */
  auto resolve(const str& token) -> tup<uint, bool, bool, bool> {
    if (is_digits(token))
      return {stoul(token), false, false, false};
    return get(token);
  }

  /** @brief Renders a formatted view of the symbol table with virtual address and value. */
  auto render_symbol_table() -> str {
    if (symbol_table.empty())
      return "  <empty>\n";
    
    auto out = osstream();
    for (auto& [var, vaddr] : symbol_table) {
      auto [value, violation, fault] = virtual_memory.read(vaddr);
      out << format("  {:<6} → vaddr={:#06x} = {:<5}", var, vaddr, value);
      if (violation) out << "  [VIOLATION]";
      if (fault)     out << "  [PAGE FAULT]";
      out << '\n';
    }
    return out.str();
  }

  // ------ Member variables ------
  umap<str,uint> symbol_table;
  MemoryView virtual_memory;
  uint next_addr;  
  uint symbol_limit;
};
