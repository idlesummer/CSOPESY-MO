#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <thread>


// === Atomic Types ===
using std::atomic;
using std::atomic_bool;
using std::atomic_int;
using std::atomic_uint;
using std::atomic_bool;

// === Locking & Threading Aliases ===
using std::lock_guard;
using std::mutex;
using std::shared_lock;
using std::shared_mutex;
using std::this_thread::sleep_for;
using std::unique_lock;
using Thread = std::thread;
