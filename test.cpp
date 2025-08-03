#include "core/memory/MemoryManager.hpp"


using namespace std;

int main() {
  auto mm = MemoryManager();
  mm.init(128, 64);       // 2 physical frames (64 words each)

  mm.alloc(1, 192);       // 3 virtual pages (64 * 3 = 192)

  auto vm = mm.memory_view_of(1);

  auto print_read = [](str label, int val, bool vio, bool fault) {
    cout << format("{:<28} | value = {:<5} | violation = {:<5} | fault = {:<5}\n",
                   label, val, vio, fault);
  };

  auto print_write = [](str label, bool vio, bool fault) {
    cout << format("{:<28} | violation = {:<5} | fault = {:<5}\n",
                   label, vio, fault);
  };

  // === CASE 1: Page fault on first access ===
  auto [v1, vio1, fault1] = vm.read(0);
  print_read("[read @0] (first access)", v1, vio1, fault1);

  // === CASE 2: Fill memory, then evict ===
  vm.write(0, 111);        // Page 0
  vm.write(64, 222);       // Page 1
  vm.write(128, 333);      // Page 2 → causes eviction
  auto [v2, vio2, fault2] = vm.read(0);  // Reload evicted page
  print_read("[reload evicted page @0]", v2, vio2, fault2);

  // === CASE 3: Read across page boundary ===
  auto [v3, vio3, fault3] = vm.read(63);  // Crosses page 0 to 1
  print_read("[read @63 across pages]", v3, vio3, fault3);

  // === CASE 4: Undeclared virtual address ===
  auto [v4, vio4, fault4] = vm.read(300);  // Outside alloc
  print_read("[read @300 undeclared]", v4, vio4, fault4);

  // === CASE 5: Write across boundary ===
  auto [vio5, fault5] = vm.write(63, 999);  // Split between page 0 and 1
  print_write("[write @63 across pages]", vio5, fault5);

  // === Optional: Dump backing store summary ===
  cout << format("\nBacking store state:\n");
  cout << format("  entries = {}\n", mm.data.store.size());
  for (auto& [key, bytes] : mm.data.store)
    cout << format("  key = {} | first_byte = {}\n", key, bytes.empty() ? -1 : bytes[0]);

  return 0;
}
