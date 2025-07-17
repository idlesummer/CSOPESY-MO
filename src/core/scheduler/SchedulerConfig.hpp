#pragma once
#include "core/common/imports/_all.hpp"


/**
 * @brief Dynamic scheduler configuration using key-value string pairs.
 * 
 * Allows easy iteration, modification, and serialization of scheduler settings.
 */
class SchedulerConfig {
  public:

  SchedulerConfig():
    entries (ordered_map<str,str>())
  {
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
  auto set(const str& key, const str& value) -> bool {
    if (!entries.contains(key)) return false;
    entries[key] = value;
    return true;
  }

  /** @brief Retrieves the string value for a given key. */
  auto get(const str& key) -> str const {
    auto it = entries.find(key);
    return it != entries.end() ? it->second : "";
  }

  /** @brief Retrieves the string value for a given key as a uint. */
  auto get_uint(const str& key) -> uint const {
    return stoul(get(key));
  }

  /** @brief Retrieves the string value for a given key as bool. */
  auto get_bool(const str& key) -> bool const {
    auto v = get(key);
    return v == "1" || v == "true";
  }

  // ------ Member variables ------
  ordered_map<str,str> entries;
};
