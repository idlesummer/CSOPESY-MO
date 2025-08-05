# This version uses MemoryView instead of VirtualMemory for process memory abstraction.

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


class MemoryManagerData:
    def __init__(self):
        self.page_size = 0          # Bytes per page/frame
        self.frame_count = 0        # Total number of frames
        self.memory = []            # Physical memory (flat byte array)
        self.free_frames = []       # List of available frame indices
        self.page_table_map = {}    # pid → PageTable
        self.equeue = []            # FIFO eviction queue: (pid, page_num)
        self.store = {}             # Backing store: uint64 key → byte array


class MemoryView:
    """
    Represents the virtual memory view for a specific process.
    Provides read/write access using virtual addresses.

    This object handles:
      - Ownership and access checks
      - Demand paging (pages are loaded into RAM on first access)
      - Read/write of 2-byte values

    ⚠️ WARNING:
        The methods `read()` and `write()` will automatically
        trigger a page-in if the requested virtual address resides
        on a page that is not yet loaded in memory.
        
        This means these methods can:
          - Modify physical memory state (RAM usage)
          - Trigger evictions (if memory is full)
          - Load pages from the backing store
    """
    def __init__(self, pid, data, page_in):
        self.pid = pid
        self.data = data
        self.page_in = page_in

    def read(self, vaddr) -> tuple[int, bool, bool]:
        """
        Read a 2-byte value from virtual memory.

        Automatically triggers paging if the page(s) are not loaded.

        Args:
            vaddr (int): The starting virtual address (2 bytes will be read).

        Returns:
            (value, is_violation, is_page_fault)
            - value: The read value (0 if failed)
            - is_violation: True if the address was invalid (out-of-bounds or unmapped)
            - is_page_fault: True if a page fault occurred during read
        """
        
        if not self._owns_vaddr(vaddr, num_bytes=2):
            return 0, True, False  # Treat as access violation (bad symbol), ignore is_pagefault

        maddr1 = self._maddr_of(vaddr)
        maddr2 = self._maddr_of(vaddr+1)
        if maddr1 is None or maddr2 is None:
            return 0, False, True  # Page fault

        lower_byte = self.data.memory[maddr1]
        upper_byte = self.data.memory[maddr2] << 8
        return lower_byte | upper_byte, False, False

    def write(self, vaddr, value) -> tuple[bool, bool]:
        """
        Write a 2-byte value to virtual memory.

        Automatically triggers paging if the page(s) are not loaded.

        Args:
            vaddr (int): Starting virtual address to write to
            value (int): 2-byte unsigned integer (0–65535)

        Returns:
            (is_violation, is_page_fault)
            - is_violation: True if write was to an invalid address
            - is_page_fault: True if a page fault occurred during write
        """
        if not self._owns_vaddr(vaddr, num_bytes=2):
            return True, False  # Access violation

        maddr1 = self._maddr_of(vaddr)
        maddr2 = self._maddr_of(vaddr+1)
        if maddr1 is None or maddr2 is None:
            return False, True  # Page fault

        self.data.memory[maddr1] = value & 0xFF
        self.data.memory[maddr2] = (value >> 8) & 0xFF
        return False, False

    # ------ PRIVATE METHODS ------

    def _owns_vaddr(self, vaddr, num_bytes=1):
        page_table = self.data.page_table_map.get(self.pid)
        if not page_table:
            return False

        start = vaddr // self.data.page_size
        end = (vaddr + num_bytes - 1) // self.data.page_size
        return all(page_table.has_page(p) for p in range(start, end + 1))

    def _maddr_of(self, vaddr):
        """
        Translate virtual address to physical address.
        Triggers demand paging if page not loaded.
        Returns None only if page_in() fails.
        """
        page_num = vaddr // self.data.page_size
        offset   = vaddr % self.data.page_size
        
        page_table = self.data.page_table_map[self.pid]
        page = page_table.get(page_num)

        if not page.is_loaded():
            success = self.page_in(self.pid, page_num)
            if not success:
                return None
            # Refresh the page reference after page_in()
            page = page_table.get(page_num) # Refresh in case of mutation

        return page.frame_num * self.data.page_size + offset


class MemoryManager:
    def __init__(self):
        self.data = MemoryManagerData()

    def init(self, capacity, page_size):
        """Initialize the memory manager with total capacity and page size."""
        self.data.page_size = page_size
        self.data.frame_count = capacity // page_size
        self.data.memory = [0] * capacity
        self.data.free_frames = list(range(self.data.frame_count))
        self.data.page_table_map.clear()

    def alloc(self, pid, bytes_needed):
        """
        Reserves virtual pages for a process without loading them.
        Pages are created but not assigned frames until accessed.
        """
        pages_needed = (bytes_needed + self.data.page_size - 1) // self.data.page_size

        page_table = PageTable()
        for page_num in range(pages_needed):
            page_table.add(page_num, PageEntry())
        self.data.page_table_map[pid] = page_table
        return True

    def get_vmemory(self, pid):
        """Returns a MemoryView for the given process."""
        return MemoryView(
            pid,
            self.data,
            page_in=lambda pid, page_num: self._page_in(pid, page_num)
        )

    # ------ PRIVATE METHODS ------

    def _fill_frame(self, frame_num, filler):
        """Fills the given frame in memory using the provided filler function."""
        maddr = frame_num * self.data.page_size
        for i in range(self.data.page_size):
            self.data.memory[maddr + i] = filler(i)

    def _page_in(self, pid, page_num):
        """
        Loads a page into memory by assigning a free frame.
        Also known as: "load_page".
        """
        frame_num = (
            self.data.free_frames.pop(0)
            if self.data.free_frames
            else self._page_out()
        )

        if frame_num is False: ########################
            return False

        page_table = self.data.page_table_map[pid]
        page = page_table.get(page_num)
        
        # Sanity check, the current page should not be in memory before page_in is called!
        assert not page.is_loaded(), "page_in() called on already-loaded page"
        
        page.frame_num = frame_num
        self.data.equeue.append((pid, page_num))

        key = self._make_key(pid, page_num)
        if key in self.data.store:
            bytes = self.data.store.pop(key)
            self._fill_frame(frame_num, lambda i: bytes[i])
            print(f"[MemoryManager._page_in()] LOAD  pid={pid} → page={page_num:<2} → frame={frame_num} (restored from store)")
        else:
            self._fill_frame(frame_num, lambda i: 0)
            print(f"[MemoryManager._page_in()] LOAD  pid={pid} → page={page_num:<2} → frame={frame_num} (zero-filled)")
        return True

    def _page_out(self):
        """
        Evicts one loaded page from memory and returns its freed frame number.
        This simulates basic page replacement when memory is full.
        Also known as: "evict_page".
        """
        while self.data.equeue:
            pid, page_num = self.data.equeue.pop(0)
            table = self.data.page_table_map[pid]
            page = table.get(page_num)

            # ✅ This *should* be loaded, otherwise we have stale data
            assert page.is_loaded(), "page_out() called on an already-evicted page"

            frame_num = page.frame_num
            page.frame_num = None
            self.data.free_frames.append(frame_num)

            # Save to store
            maddr = frame_num * self.data.page_size
            bytes_out = self.data.memory[maddr: maddr + self.data.page_size]

            key = self._make_key(pid, page_num)
            self.data.store[key] = list(bytes_out)

            print(f"[MemoryManager._page_out()] EVICT pid={pid} → page={page_num:<2} → freed frame={frame_num}")
            return frame_num

        raise Exception("Eviction failed: no pages available to evict") ########################

    def _make_key(self, pid, page_num):
        return (pid << 32) | page_num


class ProcessMemory:
    def __init__(self, view):
        self.view = view
        self.symbol_table = {}    # var name → virtual address
        self.next_offset = 0      # Next offset within symbol table
        self.symbol_limit = 64    # Max 64 bytes (→ 32 variables)

    def set(self, var, value) -> tuple[bool, bool]:
        """
        Writes a 2-byte value to a variable.

        Returns:
            is_violation (bool): True if address assignment was invalid (e.g. symbol table full)
            is_page_fault (bool): True if the write triggered a page fault
        """
        if var in self.symbol_table:
            addr = self.symbol_table[var]

        elif self.next_offset + 2 <= self.symbol_limit:
            addr = self.next_offset
            self.symbol_table[var] = addr
            self.next_offset += 2
            print(f"[ProcessMemory.set()] symbol_table['{var}'] = {addr}")

        else:
            print(f"[ProcessMemory.set()] DECLARE failed: symbol table full → skipping '{var}'")
            return True, False  # Violation, but no page fault

        return self.view.write(addr, value) # Returns (is_violation, is_page_fault)
    
    def get(self, var) -> tuple[int, bool, bool]:
        """
        Returns:
            value (int): the variable's value or 0
            is_violation (bool): True if address was invalid (e.g. undeclared or out of bounds)
            is_page_fault (bool): True if a page fault occurred
        """
        if var not in self.symbol_table:
            print(f"[ProcessMemory.get()] '{var}' not declared")
            return 0, False, True ######################## CREATE VARIABLE IF NOT CREATED   

        addr = self.symbol_table[var]
        print(f"[ProcessMemory.get()] symbol_table['{var}'] → vaddr={hex(addr)}")
        
        return self.view.read(addr) # Returns (value, is_violation, is_page_fault)
