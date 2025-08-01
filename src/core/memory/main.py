from MemoryManager import *


class CommandRegistry:
    def __init__(self):
        self.mm = MemoryManager()
        self.mm.init(capacity=128, page_size=64)
        self.mm.allocate(pid=1, bytes_needed=128)

        self.commands = {
            "process-smi": self._process_smi,
            "read": self._read,
            "write": self._write,
            "vmstat": self._vmstat,
        }

    def execute(self, line):
        tokens = line.strip().split()
        if not tokens: return
        cmd, args = tokens[0], tokens[1:]

        if cmd not in self.commands:
            return print(f"[shell] Unknown command: {cmd}")

        try: self.commands[cmd](args)
        except Exception as e:
            print(f"[error] {e}")

    # --- Command Handlers ---

    def _write(self, args):
        pid, addr, value = map(int, args)
        self.mm.write(pid, addr, value)

    def _read(self, args):
        pid, addr = map(int, args)
        print(self.mm.read(pid, addr))

    def _process_smi(self, args):
        mm = self.mm
        print("=== PROCESS-SMI: Memory Summary ===")
        print(f"Total frames:     {mm.frame_count}")
        print(f"Free frames:      {len(mm.free_frames)}")
        print(f"Used frames:      {mm.frame_count - len(mm.free_frames)}\n")

        for pid, page_table in mm.page_tables.items():
            print(f"Process PID: {pid}")
            loaded = 0
            for page_num in sorted(page_table.pages()):
                page = page_table.get(page_num)
                if page.loaded:
                    print(f"  Page {page_num:02} → Frame {page.frame}")
                    loaded += 1
                else:
                    print(f"  Page {page_num:02} → Not loaded")
            print(f"  Total pages: {len(page_table.pages())}, Loaded: {loaded}")
            print()

    def _vmstat(self, args):
      mm = self.mm
      total_frames = mm.frame_count
      free_frames = len(mm.free_frames)
      used_frames = total_frames - free_frames

      total_processes = len(mm.page_tables)
      total_vpages = 0
      total_loaded = 0

      for pid, table in mm.page_tables.items():
          total_vpages += len(table.pages())
          for page_num in table.pages():
              if table.is_loaded(page_num):
                  total_loaded += 1

      print("=== VMSTAT: System Status ===")
      print(f"Processes:           {total_processes}")
      print(f"Total frames:        {total_frames}")
      print(f"Used frames:         {used_frames}")
      print(f"Free frames:         {free_frames}")
      print()
      print(f"Total virtual pages: {total_vpages}")
      print(f"Pages loaded:        {total_loaded}")
      print(f"Pages not loaded:    {total_vpages - total_loaded}")


if __name__ == "__main__":
    registry = CommandRegistry()
    registry.execute("write 1 10 1234")
    registry.execute("write 1 60 4321")
    registry.execute("read 1 10")
    registry.execute("read 1 60")
    registry.execute("process-smi")
    registry.execute("vmstat")
