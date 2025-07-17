#pragma once
#include "core/common/imports/_all.hpp"


/**
 * @brief Dynamic scheduler configuration using key-value string pairs.
 * 
 * Allows easy iteration, modification, and serialization of scheduler settings.
 */
class SchedulerConfig {
  using map = ordered_map<str, str>;
  
  public:
  map entries;

  // === Methods ===

  /** @brief Initializes the default scheduler configuration. */
  SchedulerConfig() {
    entries["scheduler"] = "fcfs";
    entries["num-cpu"] = "1";
    entries["quantum-cycles"] = "1";
    entries["batch-process-freq"] = "1";
    entries["min-ins"] = "1";
    entries["max-ins"] = "1";
    entries["delays-per-exec"] = "0";
    entries["initialized"] = "false";
  }

  /** @brief Sets the value of a configuration key. */
  bool set(const str& key, const str& value) {
    if (!entries.contains(key)) return false;
    entries[key] = value;
    return true;
  }

  /** @brief Retrieves the string value for a given key. */
  str get(const str& key) const {
    auto it = entries.find(key);
    return it != entries.end() ? it->second : "";
  }

  /** @brief Retrieves the string value for a given key as a uint. */
  uint get_uint(const str& key) const {
    return stoul(get(key));
  }

  /** @brief Retrieves the string value for a given key as bool. */
  bool get_bool(const str& key) const {
    auto v = get(key);
    return v == "1" || v == "true";
  }
};
