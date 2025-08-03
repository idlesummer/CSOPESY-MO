class PageEntry:
    """
    Represents the metadata of a virtual page (not the actual content).

    This object tracks:
      - which physical frame it's loaded in (if any)
      - whether the page is currently loaded into memory

    Note:
        This does NOT contain the page's data. Content is stored in the
        memory array (RAM) and/or the backing store.
    """

    def __init__(self, frame_num=None):
        self.frame_num = frame_num   # Physical frame index (or None if not loaded)

    def is_loaded(self):
        """True if page is currently loaded in memory (i.e. mapped to a frame)."""
        return self.frame_num is not None


class PageTable:
    def __init__(self):
        self.entries = {}  # Maps virtual page number → PageEntry

    def has_page(self, page_num):
        return page_num in self.entries

    def get(self, page_num):
        return self.entries[page_num]

    def add(self, page_num, page):
        self.entries[page_num] = page

    def pages(self):
        return self.entries.keys()

    def mark_unloaded(self, page_num):
        self.entries[page_num].frame_num = None

    def is_loaded(self, page_num):
        return self.entries[page_num].is_loaded()

    def get_frame(self, page_num):
        return self.entries[page_num].frame_num


class MemoryManager:
    def __init__(self):
        # Config and memory state
        self.page_size = 0          # Bytes per page/frame
        self.frame_count = 0        # Total number of frames

        self.memory = []            # Physical memory: flat list of bytes
        self.free_frames = []       # Tracks available frame indices
        self.page_table_map = {}    # Page tables: maps pid → PageTable object
        self.equeue = []            # Tracks (pid, page_num) in load order
        self.store = {}             # Backing store: (pid, page_num) → byte array

    def init(self, capacity, page_size):
        """Initialize the memory manager with total capacity and page size."""
        self.page_size = page_size
        self.frame_count = capacity // page_size
        self.memory = [0] * capacity
        self.free_frames = list(range(self.frame_count))
        self.page_table_map.clear()

    def alloc(self, pid, bytes_needed):
        """
        Reserves virtual pages for a process without loading them.
        Pages are created but not assigned frames until accessed.

        Args:
            pid (int): Process ID.
            bytes_needed (int): Bytes to alloc.

        Returns:
            bool: True if allocation succeeded.
        """
        pages_needed = (bytes_needed + self.page_size - 1) // self.page_size
        page_table = PageTable()
        for page_num in range(pages_needed):
            page_table.add(page_num, PageEntry())
        self.page_table_map[pid] = page_table
        return True

    def write(self, pid, vaddr, value):
        """Write a 2-byte value at the virtual address of a process."""
        if not self._owns_vaddr(pid, vaddr, num_bytes=2):
            raise Exception(f"Access violation (WRITE) at address {hex(vaddr)}")
        maddr1 = self._maddr_of(pid, vaddr)
        maddr2 = self._maddr_of(pid, vaddr + 1)
        self.memory[maddr1] = value & 0xFF
        self.memory[maddr2] = (value >> 8) & 0xFF

    def read(self, pid, vaddr):
        """Read a 2-byte value at the virtual address of a process."""
        if not self._owns_vaddr(pid, vaddr, num_bytes=2):
            raise Exception(f"Access violation (READ) at address {hex(vaddr)}")
        maddr1 = self._maddr_of(pid, vaddr)
        maddr2 = self._maddr_of(pid, vaddr + 1)
        lo = self.memory[maddr1]
        hi = self.memory[maddr2] << 8
        return lo | hi

    # ------ PRIVATE METHODS ------

    def _owns_vaddr(self, pid, vaddr, num_bytes=1):
        """
        Checks whether a range of virtual addresses is allocated to the given process.
        Validates that [addr, addr + bytes - 1] fits entirely within
        the set of reserved pages (regardless of load state).

        Args:
            pid (int): Process ID.
            addr (int): Starting virtual address.
            bytes (int): Number of bytes to check (default: 1).

        Returns:
            bool: True if all pages in range are reserved by the process.
        """
        if pid not in self.page_table_map:
            return False
        table = self.page_table_map[pid]
        start = vaddr // self.page_size
        end = (vaddr + num_bytes - 1) // self.page_size
        return all(table.has_page(p) for p in range(start, end + 1))

    def _maddr_of(self, pid, vaddr):
        """
        Translates a virtual address to a physical memory index.
        Triggers page-in if the page is not yet loaded.

        Args:
            pid (int): Process ID.
            addr (int): Virtual address.

        Returns:
            int: Index in physical memory array.
        """
        page_num = vaddr // self.page_size
        offset = vaddr % self.page_size
        table = self.page_table_map[pid]
        page = table.get(page_num)
        if not page.is_loaded():
            self._page_in(pid, page_num)
        return page.frame_num * self.page_size + offset

    def _fill_frame(self, frame_num, filler):
        """
        Fills the given frame in memory using the provided filler function.

        Args:
            frame_num (int): Frame number to fill.
            filler (Callable[[int], int]): Function taking index (0..page_size-1) and returning a byte.
        """
        maddr = frame_num * self.page_size
        for i in range(self.page_size):
            self.memory[maddr + i] = filler(i)

    def _page_in(self, pid, page_num):
        """
        Loads a page into memory by assigning a free frame.
        Also known as: "load_page".

        Case 0: If the page is not loaded, this assigns a frame and zero-fills it.
        Simulates demand paging (no actual disk access).
        
        Case 1: If memory is full, this triggers page replacement via FIFO eviction.

        Args:
            pid (int): Process ID.
            page_num (int): Virtual page number to load.

        Raises:
            Exception: If no free frames are available.
        """
        frame_num = self.free_frames.pop(0) if self.free_frames else self._page_out()
        table = self.page_table_map[pid]
        page = table.get(page_num)
        page.frame_num = frame_num
        self.equeue.append((pid, page_num))

        key = (pid, page_num)
        if key in self.store:
            data = self.store.pop(key)
            self._fill_frame(frame_num, lambda i: data[i])
            print(f"[MemoryManager._page_in()] LOAD  pid={pid} → page={page_num:<2} → frame={frame_num} (restored from store)")
        else:
            self._fill_frame(frame_num, lambda i: 0)
            print(f"[MemoryManager._page_in()] LOAD  pid={pid} → page={page_num:<2} → frame={frame_num} (zero-filled)")

    def _page_out(self):
        """
        Evicts one loaded page from memory and returns its freed frame number.
        This simulates basic page replacement when memory is full.
        Also known as: "evict_page".

        Returns:
            int: A frame number that was reclaimed.
        """
        while self.equeue:
            pid, page_num = self.equeue.pop(0)
            table = self.page_table_map[pid]
            page = table.get(page_num)
            
            if not page.is_loaded():
                continue
            
            frame_num = page.frame_num
            page.frame_num = None
            self.free_frames.append(frame_num)
            
            print(f"[MemoryManager._page_out()] EVICT pid={pid} → page={page_num:<2} → freed frame={frame_num}")
            return frame_num
        raise Exception("Eviction failed: no pages available to evict")


class VirtualMemory:
    """
    Represents the virtual memory view for a specific process.
    Provides read/write access using virtual addresses,
    delegating to the system-wide MemoryManager.
    """
    def __init__(self, pid, manager):
        self.pid = pid
        self.manager = manager

    def read(self, vaddr):
        """Read a 2-byte value from the process's virtual memory."""
        return self.manager.read(self.pid, vaddr)

    def write(self, vaddr, value):
        """Write a 2-byte value to the process's virtual memory."""
        self.manager.write(self.pid, vaddr, value)

class ProcessMemory:
    def __init__(self, vmemory):
        self.vmemory = vmemory
        self.symbol_table = {}    # var name → virtual address
        self.next_offset = 0      # Next offset within symbol table
        self.symbol_limit = 64    # Max 64 bytes (→ 32 symbol_table)

    def set(self, var, value):
        if var in self.symbol_table:
            addr = self.symbol_table[var]
        elif self.next_offset + 2 <= self.symbol_limit:
            addr = self.next_offset
            self.symbol_table[var] = addr
            self.next_offset += 2
            print(f"[ProcessMemory.set()] symbol_table['{var}'] = {addr}")
        else:
            print(f"[ProcessMemory.set()] DECLARE failed: symbol table full → skipping '{var}'")
            return
        self.vmemory.write(addr, value)

    def get(self, var):
        if var not in self.symbol_table:
            print(f"[ProcessMemory.get()] '{var}' not declared — returning 0")
            return 0
        addr = self.symbol_table[var]
        print(f"[ProcessMemory.get()] symbol_table['{var}'] → vaddr={hex(addr)}")
        return self.vmemory.read(addr)


# mm = MemoryManager()
# mm.init(capacity=128, page_size=64)       # Creates 2 frames
# mm.alloc(pid=1, bytes_needed=64)       # Allocates 1 page (for symbol table)

# vmemory = VirtualMemory(pid=1, manager=mm)
# process = Process()
# process.memory = ProcessMemory(vmemory)

# DECLARE(Instruction(["a", "100"]), process)
# DECLARE(Instruction(["b", "250"]), process)
# ADD(Instruction(["c", "a", "b"]), process)