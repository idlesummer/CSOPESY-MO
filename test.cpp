#include "core/memory/MemoryManager.hpp"

int main() {
  auto mm = MemoryManager();
  mm.init(128, 64);                     // 2 physical frames (64 words each)

  auto vm = mm.memory_view_of(1, 192u); // 3 virtual pages (64 * 3 = 192)

  auto print_read = [](str label, int val, bool vio, bool fault) {
    cout << format("  {:<28} | value = {:<5} | violation = {:<5} | fault = {:<5}\n\n",
                   label, val, vio, fault);
  };

  auto print_write = [](str label, bool vio, bool fault) {
    cout << format("  {:<28} | violation = {:<5} | fault = {:<5}\n\n",
                   label, vio, fault);
  };

  // === CASE 1: Page fault on first access ===
  cout << "\n[MARK] read @0\n";
  auto [v1, vio1, fault1] = vm.read(0);
  print_read("[read @0] (first access)", v1, vio1, fault1);

  // === CASE 2: Fill memory, then evict ===
  cout << "\n[MARK] write @0\n";
  vm.write(0, 111);        // Page 0

  cout << "\n[MARK] write @64\n";
  vm.write(64, 222);       // Page 1 

  cout << "\n[MARK] write @128\n";
  vm.write(128, 333);      // Page 2 → causes eviction

  cout << "\n[MARK] read @0 again\n";
  auto [v2, vio2, fault2] = vm.read(0);  // Reload evicted page
  print_read("[reload evicted page @0]", v2, vio2, fault2);

  // === CASE 3: Read across page boundary ===
  cout << "\n[MARK] read @63\n";
  auto [v3, vio3, fault3] = vm.read(63);  // Crosses page 0 to 1
  print_read("[read @63 across pages]", v3, vio3, fault3);

  // === CASE 4: Undeclared virtual address ===
  cout << "\n[MARK] read @300\n";
  auto [v4, vio4, fault4] = vm.read(300);  // Outside alloc
  print_read("[read @300 undeclared]", v4, vio4, fault4);

  // === CASE 5: Write across boundary ===
  cout << "\n[MARK] write @63\n";
  auto [vio5, fault5] = vm.write(63, 999);  // Split between page 0 and 1
  print_write("[write @63 across pages]", vio5, fault5);

  // === Optional: Dump backing store summary ===
  cout << format("\nBacking store state:\n");
  cout << format("  entries = {}\n", mm.data.store.size());
  for (auto& [key, bytes] : mm.data.store) {
    auto pid = key >> 32;
    auto page_num = key & 0xFFFFFFFF;
    auto val = bytes[0] | (bytes[1] << 8); // reconstruct 16-bit value
    cout << format("  pid = {:<2} page = {:<2} | value = {}\n", pid, page_num, val);
  }

  cout << "\n";
  return 0;
}
