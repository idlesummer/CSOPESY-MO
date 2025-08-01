
class Page:
    def __init__(self, frame_num=None, loaded=False):
        self.frame = frame_num   # Physical frame index (or None if not loaded)
        self.loaded = loaded     # True if page is currently loaded in memory


class PageTable:
    def __init__(self):
        self.entries = {}  # Maps virtual page number → Page

    def has_page(self, page_num):
        return page_num in self.entries

    def get(self, page_num):
        return self.entries[page_num]

    def add(self, page_num, page):
        self.entries[page_num] = page

    def pages(self):
        return self.entries.keys()

    def mark_unloaded(self, page_num):
        self.entries[page_num].loaded = False

    def is_loaded(self, page_num):
        return self.entries[page_num].loaded

    def get_frame(self, page_num):
        return self.entries[page_num].frame


class MemoryManager:
    def __init__(self):
        # Config and memory state
        self.page_size = 0          # Bytes per page/frame
        self.frame_count = 0        # Total number of frames

        self.memory = []            # Physical memory: flat list of bytes
        self.free_frames = []       # Tracks available frame indices
        self.page_tables = {}       # Page tables: maps pid → PageTable object
        self.eviction_queue = []    # Tracks (pid, page_num) in load order

    def init(self, capacity, page_size):
        """Initialize the memory manager with total capacity and page size."""
        self.page_size = page_size
        self.frame_count = capacity // page_size

        self.memory = [0] * capacity
        self.free_frames = list(range(self.frame_count))
        self.page_tables.clear()

    def allocate(self, pid, bytes_needed):
        """
        Reserves virtual pages for a process without loading them.
        Pages are created but not assigned frames until accessed.

        Args:
            pid (int): Process ID.
            bytes_needed (int): Bytes to allocate.

        Returns:
            bool: True if allocation succeeded.
        """
        
        # Equivalent to `pages_needed = ceil(bytes_needed / page_size)`
        pages_needed = (bytes_needed + self.page_size - 1) // self.page_size
        page_table = PageTable()

        for page_num in range(pages_needed):
            page_table.add(page_num, Page())  # Lazy allocation
            
        self.page_tables[pid] = page_table
        return True

    def write(self, pid, vaddr, value):
        """Write a 2-byte value at the virtual address of a process."""
        if not self._owns_vaddr(pid, vaddr, num_bytes=2):
            raise Exception(f"Access violation (WRITE) at address {hex(vaddr)}")

        maddr1 = self._maddr_of(pid, vaddr)
        maddr2 = self._maddr_of(pid, vaddr + 1)

        self.memory[maddr1] = value & 0xFF          # Separate lower byte into maddr1
        self.memory[maddr2] = (value >> 8) & 0xFF   # Separate upper byte into maddr2

    def read(self, pid, vaddr):
        """Read a 2-byte value at the virtual address of a process."""
        if not self._owns_vaddr(pid, vaddr, num_bytes=2):
            raise Exception(f"Access violation (READ) at address {hex(vaddr)}")

        maddr1 = self._maddr_of(pid, vaddr)
        maddr2 = self._maddr_of(pid, vaddr + 1)

        lower_byte = self.memory[maddr1]        # Retrieve lower byte
        upper_byte = self.memory[maddr2] << 8   # Retrieve upper byte shifted left
        value = lower_byte | upper_byte         # Combine both lower and upper byte
        return value


    # ------ PRIVATE METHHODS ------


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
        if pid not in self.page_tables:
            return False

        page_table = self.page_tables[pid]
        start_page = vaddr // self.page_size                    # e.g., addr=60 → page 0
        end_page = (vaddr + num_bytes - 1) // self.page_size    # e.g., addr=60, bytes=4 (60,61,62,63) → last byte = 63 → page 0

        for page_num in range(start_page, end_page + 1):
            if not page_table.has_page(page_num):
                return False
        return True

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
        page_num = vaddr // self.page_size               # virtual page number
        offset = vaddr % self.page_size                  # byte offset within the page

        page_table = self.page_tables[pid]
        page = page_table.get(page_num)

        if not page.loaded:
            self._page_in(pid, page_num)                # 🔥 Lazy page-in on fault

        frame = page.frame
        return frame * self.page_size + offset

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

        frame = None
        if self.free_frames:
            frame = self.free_frames.pop(0)
        else:
            # Reclaim a frame from an existing loaded page
            frame = self._page_out()

        page_table = self.page_tables[pid]  # Get page table from registry
        page = page_table.get(page_num)     # Get page from page table
        page.frame = frame
        page.loaded = True

        # Add to eviction tracking (FIFO)
        self.eviction_queue.append((pid, page_num))

        # Compute the starting maddr (byte offset) of the frame
        # Each frame is a contiguous block of `page_size` bytes
        maddr = frame * self.page_size  
        
        # Zero-fill the entire frame to simulate a clean page load
        for i in range(self.page_size): 
            self.memory[maddr + i] = 0

    def _page_out(self):
        """
        Evicts one loaded page from memory and returns its freed frame number.
        This simulates basic page replacement when memory is full.
        Also known as: "evict_page".

        Returns:
            int: A frame number that was reclaimed.
        """
        

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


# mm = MemoryManager()
# mm.init(capacity=128, page_size=64)     # 2 frames: [0-63] and [64-127]
# mm.allocate(pid=0, bytes_needed=64)     # process0: 0-63 virtual memory
# mm.allocate(pid=1, bytes_needed=64)     # process1:             0-63 virtual memory

# vm0 = VirtualMemory(pid=0, manager=mm)
# vm0.write(10, 1234)
# print(vm0.read(10))

# vm1 = VirtualMemory(pid=1, manager=mm)
# vm1.write(10, 4321)
# print(vm1.read(10))
