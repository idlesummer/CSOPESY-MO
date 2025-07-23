#pragma once
#include "core/common/imports/_all.hpp"


/**
 * @brief Dynamic scheduler configuration using key-value pairs with std::any.
 * 
 * Supports storing any type but only str, uint, and bool are officially supported.
 */
class Config {
  public:

  Config():
    entries (ordered_map<str,any>())
  {
    set("scheduler", "fcfs"s);
    set("num-cpu", 1u);
    set("quantum-cycles", 1u);
    set("batch-process-freq", 1u);
    set("min-ins", 1u);
    set("max-ins", 1u);
    set("delays-per-exec", 0u);
    set("initialized", false);
  }

  /** @brief Sets the value of a configuration key. */
  void set(const str& key, any value) {
    entries[key] = move(value);
  }

  /** @brief Gets the value of a configuration key with expected type. */
  template <typename Type>
  auto get(const str& key) -> Type {
    if (!entries.contains(key))
      throw runtime_error(format("Missing config key '{}'", key));

    try {
      return cast<Type>(entries.at(key));

    } catch (bad_any_cast& e) {
      throw runtime_error(format("Config key '{}' has wrong type: {}", key, e.what()));
    }
  }

  /** @brief Returns true if key exists. */
  auto has(const str& key) const -> bool {
    return entries.contains(key);
  }

  // ------ Getters ------
  
  auto all() -> ordered_map<str,any>& { return entries; }

  /** @brief Retrieves the value of a key as a string. */
  auto gets(const str& key) -> str { return get<str>(key); }

  /** @brief Retrieves the value of a key as an unsigned integer. */
  auto getu(const str& key) -> uint { return get<uint>(key); }
  
  /** @brief Retrieves the value of a key as a boolean. */
  auto getb(const str& key) -> bool { return get<bool>(key); }

  // ------ Static Helpers ------

  /** @brief Parses a raw string into a typed value (bool, uint, or str). */
  static auto parse(const str& raw) -> any {
    // Lowercase copy for comparison
    auto lower = lowercase(raw);

    // Check if it's a boolean
    if (lower == "true")  return true;
    if (lower == "false") return false;

    // Check if it's a number (digits only)
    if (is_digits(raw))
      return cast<uint>(stoul(raw));

    // Default to string
    return raw;
  }

  private:

  // ------ Member variables ------
  ordered_map<str,any> entries;
};
