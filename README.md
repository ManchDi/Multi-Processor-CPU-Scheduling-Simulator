# Multi-Processor CPU Scheduling Simulator

This project is a **multi-processor CPU scheduling simulator** written in C++ using **POSIX threads (pthreads)**. It supports multiple scheduling algorithms and demonstrates process execution across multiple processors.

---

## Features

* Supports 4 scheduling algorithms:

  * **FCFS** (First Come First Served)
  * **Round Robin (RR)** with time quantum
  * **SJF** (Shortest Job First)
  * **Priority-based scheduling**
* Simulates execution of processes with CPU burst times, priority, and process metadata.
* Multi-threaded: each processor runs as a separate thread.
* Load balancing: assign processes to multiple processors using load percentages.
* Pretty-printed, aligned output for easy reading.

---

## Technologies

* **C++17**
* **POSIX Threads (pthread)**
* Standard C++ libraries: `<iostream>`, `<thread>`, `<queue>`, `<mutex>`, `<fstream>`

---

## Usage

### Compile

```bash
g++ -std=c++17 -pthread scheduler.cpp -o scheduler
```

### Run

```bash
./scheduler <processFile> <algorithm load pairs>
```

**Arguments:**

* `<processFile>`: binary file containing `Process` structs.
* `<algorithm load pairs>`: pairs of algorithm index and load for each processor.

**Algorithm Index Mapping:**

* `0` = FCFS
* `1` = Round Robin
* `2` = SJF
* `3` = Priority

**Example:**

```bash
./scheduler processesB_Spring2023.bin 0 0.25 1 0.25 2 0.25 3 0.25
```

This runs 4 processors:

* Processor 1: FCFS, 25% load
* Processor 2: RR, 25% load
* Processor 3: SJF, 25% load
* Processor 4: Priority, 25% load

---

## Output Format

Example:

```
Processor_1 | FCFS     | PID: 0   | Process_0  | Exec: 87  | Left: 0
Processor_2 | RR       | PID: 13  | Process_13 | Exec: 2   | Left: 0
Processor_3 | SJF      | PID: 31  | Process_31 | Exec: 2   | Left: 9
Processor_4 | Priority | PID: 42  | Process_42 | Exec: 2   | Left: 49 | Pri: 16
```

Columns:

* **Processor_X** – processor index
* **Algorithm** – scheduling algorithm
* **PID** – process ID
* **Process name** – process identifier
* **Exec** – CPU burst executed in this step
* **Left** – remaining CPU burst
* **Pri** – priority (only for Priority scheduling)

---

## File Format

The simulator expects a **binary file** of `Process` structs:

```cpp
struct Process {
    char processName[32];
    int processId;
    char activityStatus;
    int cpuBurst;
    int baseRegister;
    int64_t limitRegister;
    char processType;
    int numberFiles;
    char priority;
    int checksum;
};
```

---

## Notes

* Time quantum for Round Robin and other algorithms is fixed at `2` units (configurable in code).
* Output is aligned and easy to read, suitable for demos or grading.
* Can be extended to include **dynamic time quantum, logging, or GUI visualization**.

---

## Author

Dmitrii Manchurak
Personal project / OS Scheduling Lab 5

---

Do you want me to **also include a “Demo GIF or Example Run” section** in the README for visual clarity?
