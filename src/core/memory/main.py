from MemoryManager2 import *


class Process:
    def __init__(self, pid, manager):
        vmemory = manager.get_vmemory(pid)
        self.memory = ProcessMemory(vmemory)


class CommandRegistry:
    def __init__(self, mm: MemoryManager, process: Process):
        self.mm = mm
        self.process = process
        self.commands = {
            "write": self._write,
            "read": self._read,
            "declare": self._declare,
            "add": self._add,
            "vmstat": self._vmstat,
            "process-smi": self._process_smi,
            "write-raw": self._write_raw,
        }

    def execute(self, line):
        tokens = line.strip().split()
        if not tokens: return
        cmd, args = tokens[0], tokens[1:]

        if cmd not in self.commands:
            print(f"[shell] Unknown command: {cmd}")
            return

        try:
            self.commands[cmd](args)
        except Exception as e:
            print(f"[error] {e}")

    # Command Handlers

    def _write_raw(self, args):
        if len(args) != 2:
            print("[write-raw] Usage: write-raw <vaddr> <value>")
            return

        vaddr = int(args[0])
        value = int(args[1])
        print(f"[exec] WRITE-RAW vaddr={hex(vaddr)} = {value}")

        is_violation, is_pagefaulted = self.process.memory.view.write(vaddr, value)
        if is_violation:
            print("[write-raw] Invalid address (violation)")
        elif is_pagefaulted:
            print("[write-raw] Page fault occurred — retry later")
        else:
            print("[write-raw] Write successful")

    def _write(self, args):
        var, value = args[0], int(args[1])
        print(f"[exec] WRITE {var} = {value}")
        success = self.process.memory.set(var, value)
        if not success:
            print("[write] Page fault occurred — retry later")

    def _read(self, args):
        var = args[0]
        print(f"[exec] READ {var}")
        value, is_violation, is_pagefaulted = self.process.memory.get(var)

        if is_violation:
            print(f"[read] '{var}' is undeclared — cannot read")
        elif is_pagefaulted:
            print("[read] Page fault occurred — retry later")

    def _declare(self, args):
        if len(args) != 2:
            print("[declare] Usage: declare <var> <value>")
            return

        var, value = args[0], int(args[1])
        print(f"[exec] DECLARE {var} = {value}")
        success = self.process.memory.set(var, value)
        if not success:
            print(f"[declare] Page fault occurred during declaration — retry later")

    def _add(self, args):
        if len(args) != 3:
            print("[add] Usage: add <dst> <src1> <src2>")
            return
        dst, src1, src2 = args
        print(f"[exec] ADD {dst} {src1} {src2}")

        val1, inv1, pf1 = self.process.memory.get(src1)
        val2, inv2, pf2 = self.process.memory.get(src2)

        if inv1 or inv2:
            print("[add] One or more variables undeclared — aborting")
            return
        if pf1 or pf2:
            print("[add] Page fault occurred — retry later")
            return

        result = (val1 + val2) & 0xFFFF
        success = self.process.memory.set(dst, result)
        if not success:
            print("[add] Destination write page fault — retry later")

    def _vmstat(self, args):
        if not args:
            print("[vmstat] Usage: vmstat <pid>")
            return

        pid = int(args[0])
        if pid not in self.mm.data.page_table_map:
            print(f"[vmstat] No page table found for pid={pid}")
            return

        print(f"[vmstat] Process {pid}:")
        table = self.mm.data.page_table_map[pid]
        for page_num in sorted(table.pages()):
            entry = table.get(page_num)
            if entry.is_loaded():
                print(f"  Page {page_num:<2} → Frame {entry.frame_num}")
            else:
                print(f"  Page {page_num:<2} → Not Loaded")

    def _process_smi(self, args):
        mm = self.mm
        print("=== PROCESS-SMI: Memory Summary ===")
        print(f"Total frames:     {mm.data.frame_count}")
        print(f"Free frames:      {len(mm.data.free_frames)}")
        print(f"Used frames:      {mm.data.frame_count - len(mm.data.free_frames)}\n")

        for pid, page_table in mm.data.page_table_map.items():
            print(f"Process PID: {pid}")
            loaded = 0
            for page_num in sorted(page_table.pages()):
                page = page_table.get(page_num)
                if page.is_loaded():
                    print(f"  Page {page_num:02} → Frame {page.frame_num}")
                    loaded += 1
                else:
                    print(f"  Page {page_num:02} → Not loaded")
            print(f"  Total pages: {len(page_table.pages())}, Loaded: {loaded}")
            print()


if __name__ == "__main__":
    mm = MemoryManager()
    mm.init(capacity=64, page_size=64)
    mm.alloc(pid=1, bytes_needed=128)        # 2 pages
    process = Process(pid=1, manager=mm)
    registry = CommandRegistry(mm, process)

    registry.execute("write-raw 0 111")      ; print()  # page 0 → frame 0
    registry.execute("write-raw 64 222")     ; print()  # page 1 → frame 0
    registry.execute("write-raw 0 333")      ; print()  # will fail naturally

