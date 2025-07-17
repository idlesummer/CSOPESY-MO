#pragma once
#include "core/common/imports/_all.hpp"


/** A dynamic runtime key-value store for shell commands and internal state. */
class ShellStorage {
  using map = unordered_map<str, any>;
  
  map storage;

  public:

  /** Set a value in shell's dynamic storage. */
  template <typename Type>
  void set(const str& key, Type value) {
    storage[key] = move(value);
  }

  /** Get a reference to a stored value. Throws if key/type is incorrect. */
  template <typename Type>
  Type& get(const str& key) {
    try { 
      return any_cast<Type&>(storage.at(key)); 
    } 
    catch (const out_of_range&) {
      throw runtime_error(format("ShellStorage: Missing key '{}'", key));
    } 
    catch (const bad_any_cast&) {
      throw runtime_error(format("ShellStorage: Bad type cast for key '{}'", key));
    }
  }

  /** Check if a key exists in the storage. */
  bool has(const str& key) const {
    return storage.contains(key);
  }

  /** Remove a key from storage. */
  void remove(const str& key) {
    storage.erase(key);
  }
};
