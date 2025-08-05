# CSOPESY-MO: Shell-based OS Process Execution Simulator

A multithreaded shell-based operating system simulation that models process scheduling, memory management, and instruction execution with virtual paging, memory snapshots, and CLI-based commands. Designed for educational purposes to demonstrate operating system internals in C++.

---

## How to Build and Run
# 1. Clone the repository
git clone https://github.com/idlesummer/CSOPESY-MO.git
cd CSOPESY-MO

# 2. Compile the project using make
make OR g++ -std=c++20 -I src/ -o main src/main.cpp && main

# 3. Run the executable
./main
---

## 🚀 Features

### ✅ Shell & Command Interface
- Interactive Shell with CLI-like command parsing

#### Core Commands:
| Command                                   | Description                              |
| ----------------------------------------- | ---------------------------------------- |
| help                                    | View command list and descriptions       |
| exit                                    | Exit the simulation                      |
| cls                                     | Clear the screen                         |
| config                                  | View system configuration                |
| vmstat                                  | Show memory statistics                   |
| process-smi                             | Show process and memory usage summary    |
| report-util                             | Generate CPU utilization reports         |
| scheduler-start                         | Start the scheduler thread               |
| scheduler-stop                          | Stop the scheduler thread                |
| screen -s <name> <mem>                  | Create new process screen                |
| screen -c <name> <mem> "<instructions>" | Create process from inline instructions  |
| screen -r <name>                        | Resume a previously created process      |
| screen -v <name>                        | Visualize process memory and stack       |
| screen -ls                              | Show all running and finished processes  |
| script <path>                           | Run a script file as a batch job         |
| print <symbol>                          | View value of symbol from virtual memory |
| log                                     | View the logs of the current process     |
| initialize                              | Reinitialize the system memory/scheduler |
| demo                                    | Run a demo mode                          |
| echo <text>                             | Print text to the shell                  |
| args                                    | Show current command arguments           |

### ✅ Memory Management
- *Virtual Memory with Paging*
- *Configurable Page Size and Memory Capacity*
- *Automatic Page Allocation*
- *Page Eviction and Backing Store* using FIFO queue
- *Memory Snapshots* every 4 quantum ticks
- *Per-process Memory Views*

### ✅ Process & Instruction Model
- *Custom Instruction Set*: DECLARE, ADD, SUBTRACT, WRITE, READ, PRINT
- *Instruction Parsing & Execution*
- *Per-process Program with Logs and Instruction Pointer*
- *Symbol Table stored in Virtual Memory*

### ✅ Scheduling
- *Round Robin or FCFS Scheduler (based on config.txt)*
- *Time Quantum (Default: 4)*
- *Context Switching*
- *Supports Multiple Simultaneous Processes*

---

## 🧵 Thread Architecture

| Thread Type     | Description                                                                |
|-----------------|----------------------------------------------------------------------------|
| Shell Thread    | Handles user input, command execution, and CLI interface                   |
| Scheduler Thread| Manages time slices and selects which process executes next                |
| Core Threads    | Simulate actual instruction execution per process                          |

🔒 *All threads are serialized*, meaning *only one executes at a time*. This avoids race conditions and keeps memory and process states consistent without requiring locks or atomic primitives.

---

## 🧪 Example Instruction

text
DECLARE A 5;
ADD A 3;
WRITE A;
PRINT A;

Each instruction operates on virtual memory, triggering page-ins and possible evictions.

---

## 🖥️ Output Example (process-smi)

------------------------------------------------------------
| PROCESS-SMI V01.00 Driver Version: 01.00                 |
------------------------------------------------------------
CPU-Util:  100%
Memory Usage: 64MiB / 128MiB
Memory Util: 50%

------------------------------------------------------------
Running processes and memory usage:

process00 4MiB
process01 8MiB

---

## 🛠️ Configuration
You can customize system behavior via the config command or by editing the config file.

Default Configuration:
| Setting              | Value | Description                               |
| -------------------- | ----- | ----------------------------------------- |
| num-cpu            | 2     | Number of simulated CPU cores             |
| scheduler          | fcfs  | Scheduling algorithm (fcfs, rr, etc.) |
| quantum-cycles     | 0     | Quantum cycles (used in Round-Robin)      |
| batch-process-freq | 1     | Frequency (in cycles) to batch processes  |
| min-ins            | 15    | Minimum instruction count per process     |
| max-ins            | 15    | Maximum instruction count per process     |
| delays-per-exec    | 50    | Simulated delay per instruction           |
| max-overall-mem    | 512   | Maximum memory across all processes (KiB) |
| mem-per-frame      | 256   | Frame size in bytes                       |
| min-mem-per-proc   | 512   | Minimum memory per process (KiB)          |
| max-mem-per-proc   | 512   | Maximum memory per process (KiB)          |

Use the command config inside the shell to view current values.

## 📦 File Structure (Relevant)

core/
  ├── shell/
  ├── process/
  ├── memory/
  ├── execution/
  └── command/
tests/
resources/
README.md


## 👨‍💻 Contributors
- Nash Luis Maramag
- Isaac Alipon Dicayanan
- Alliyah Bianca Maunahan
- Raine Miguel Villaver

---