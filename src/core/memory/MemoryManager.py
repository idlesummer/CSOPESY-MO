class MemoryManager:
    def __init__(self):
        self.frame_size = 0     # Frame and memory info
        self.total_frames = 0   # Simulated physical memory (flat list of bytes)
        self.memory = []        # Queue of available frame indices
        self.free_frames = []

        # Page table: maps each process ID to its virtual pages
        # Each page maps to a (frame_index, loaded) tuple
        self.page_table = {}

    def init(self, mem_size, frame_size):
        """Reinitialize memory with a new total size and frame size."""
        self.frame_size = frame_size
        self.total_frames = mem_size // frame_size

        self.memory = [0] * mem_size
        self.free_frames = list(range(self.total_frames))
        self.page_table.clear()

    def allocate(self, pid, size):
        """
        Allocate memory for a process.
        Divides the requested size into pages and maps each to a free frame.
        """
        pages = (size + self.frame_size - 1) // self.frame_size
        if pages > len(self.free_frames):
            return False  # Not enough frames available

        self.page_table[pid] = {}
        for page_number in range(pages):
            frame_index = self.free_frames.pop(0)
            self.page_table[pid][page_number] = (frame_index, True)
        return True

    def is_valid(self, pid, address):
        """Check if a given virtual address is valid for the process."""
        page = address // self.frame_size
        return pid in self.page_table and page in self.page_table[pid]

    def _get_index(self, pid, address):
        """
        Translate a virtual address into a physical memory index.
        Requires the page to be valid and loaded.
        """
        page = address // self.frame_size
        offset = address % self.frame_size
        frame, _ = self.page_table[pid][page]
        return frame * self.frame_size + offset

    def write(self, pid, address, value):
        """Write a 2-byte integer value to a virtual address."""
        if not self.is_valid(pid, address):
            raise Exception(f"Access violation (WRITE) at address {hex(address)}")
        index = self._get_index(pid, address)
        self.memory[index] = value & 0xFF               # lower byte
        self.memory[index + 1] = (value >> 8) & 0xFF    # upper byte

    def read(self, pid, address):
        """Read a 2-byte integer value from a virtual address."""
        if not self.is_valid(pid, address):
            raise Exception(f"Access violation (READ) at address {hex(address)}")
        index = self._get_index(pid, address)
        return self.memory[index] + (self.memory[index + 1] << 8)


mm = MemoryManager()                        # Sample usage and test
mm.reset(mem_size=256, frame_size=64)       # Set up memory: 256 bytes total, 64 bytes per frame → 4 frames
mm.allocate(pid=1, size=128)                # Allocate 128 bytes to process 1 → needs 2 frames (128 / 64 = 2)
mm.write(pid=1, address=0x0040, value=1337) # Write a 2-byte value to virtual address 0x0040
print(mm.read(pid=1, address=0x0040))       # Read it back from the same address. Should output: 1337
