#pragma once
#include "core/common/imports/_all.hpp"

namespace csopesy {
  struct EventHandler {
    using func = function<void(const any&)>;
    uint id;
    func fn;
  };

  /**
   * @class EventEmitter
   * @brief Thread-safe event emitter system.
   * 
   * Enables multiple threads to register event handlers, emit named events 
   * with optional data, and process the queued events. Supports blocking wait 
   * for event availability.
   */
  class EventEmitter {
    using atomic = atomic_uint;
    using pair = pair<str, any>;
    using list = vector<pair>;
    using obj  = EventHandler;
    using func = EventHandler::func;
    using task = function<void()>;
    using objs = vector<obj>;
    using map  = unordered_map<str, objs>;

    map    listeners;   ///< Map of event name to handlers
    list   events;      ///< Queue of emitted events
    convar cv;          ///< Condition variable for blocking waits
    atomic next_id = 1; ///< Atomic counter for unique handler IDs
    mutex  mtx;         ///< Mutex for thread safety

    public:
    
    /**
     * @brief Register a handler for a named event.
     * @param name The event name.
     * @param handler A function to call when the event is dispatched.
     * @return uint The unique handler ID assigned to the registered handler.
     */
    uint on(str name, func handler) {
      uint id = next_id++;
      return on(id, move(name), move(handler));
    }

    /**
     * @brief Register a simple handler for a named event with no payload.
     * @param name The event name.
     * @param handler A function to call when the event is dispatched (ignores any payload).
     * @return The unique handler ID assigned to this registration.
     */
    uint on(str name, task handler) {
      return on(move(name), [handler=move(handler)](const any&) { 
        handler(); 
      });
    }

    /**
     * @brief Register a one-time handler for a named event.
     * The handler will be called once, then automatically unregistered.
     * 
     * @param name The event name.
     * @param handler The function to call once.
     */
    void once(str name, func handler) {
      uint id = next_id++; 
      on(id, name, [this, id, name=move(name), handler=move(handler)](const any& payload) {
        off(name, id);    // passes name by value to on(), 
        handler(payload); // then moves into lambda capture
      });
    }

    /**
     * @brief Unregister a handler by ID for a specific event.
     * @param name The event name.
     * @param id The handler ID to remove.
     */
    void off(const str& name, uint id) {
      with.lock(mtx, [&] {
        auto it = listeners.find(name);
        if (it != listeners.end())
          erase_if(it->second, [&](const obj& h) { return h.id == id; });
      });
    }

    /**
     * @brief Unregister all handlers for a specific event.
     * @param name The event name for which all handlers will be removed.
     */
    void off(const str& name) {
      with.lock(mtx, [&] {
        auto it = listeners.find(name);
        if (it != listeners.end())
          listeners.erase(it);          // Remove key and its values from map completely O(1) avg
      });
    }

    /**
     * @brief Emit an event with data.
     * Queues the event and notifies any thread waiting for events.
     * 
     * @param name The event name.
     * @param payload The data to pass to handlers.
     */
    void emit(str name, any payload={}) {
      with.lock(mtx, [&] { 
        events.emplace_back(move(name), move(payload));
      });
      cv.notify_one();
    }

    /**
     * @brief Dispatches all queued events to their registered handlers.
     * 
     * Safely callable from any thread. This method swaps out the event queue
     * under a lock and processes a copy of it outside the lock to avoid
     * blocking other operations. For each event, the list of handlers is
     * also copied under a lock to prevent concurrent modification during
     * dispatch. This ensures thread-safe and non-blocking execution of
     * handler callbacks, even if new handlers are added or removed during dispatch.
     */
    void dispatch() {
      list local;
      with.lock(mtx, [&] { local.swap(events); });

      for (const auto& [name, data]: local) {
        objs handlers;

        with.lock(mtx, [&] {
          auto it = listeners.find(name);
          if (it != listeners.end() && !it->second.empty()) // Second condition avoids copying empty list
            handlers = it->second;      // Cant pass by reference cuz handlers is unsafe after lock
        });

        if (handlers.empty()) continue; // Skip if there are no handlers
        for (const auto& handler: handlers) 
          handler.fn(data);
      }
    }

    /**
     * @brief Block until at least one event is queued, then dispatch it.
     * Uses a condition variable to efficiently wait until work is available.
     */
    void wait_dispatch() {
      with.unique(mtx, [&](auto& lock) {
        cv.wait(lock, [&] { return !events.empty(); });
      });
      dispatch();
    }

    private:
    /**
     * @brief Internal registration helper to associate a handler with an explicit ID.
     * @param id A preassigned handler ID.
     * @param name The event name.
     * @param handler The function to register.
     * @return The same ID passed in.
     */
    uint on(uint id, str name, func handler) {
      with.lock(mtx, [&] { 
        listeners[move(name)].emplace_back(id, move(handler)); 
      });
      return id;
    }
  };
}
