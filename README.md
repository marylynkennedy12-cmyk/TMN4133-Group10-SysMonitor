# TMN4133 - Group 10 - SysMonitor++

**Linux System Resource Monitoring Tool**

A command-line system monitoring tool written in C that displays real-time CPU usage, memory usage, and process information using Linux system calls and the /proc filesystem.

---

## 📋 Project Overview

This project was developed as part of the TMN4133 System Programming course requirements. SysMonitor++ is a lightweight monitoring tool that provides system administrators with essential resource usage information without the overhead of tools like htop.

**Course:** TMN4133 System Programming  
**Institution:** Universiti Malaysia Sarawak  
**Submission Date:** Friday, 19th December 2025  
**Group:** Group 10

---

## ✨ Features

### Core Functionality
- ✅ **CPU Usage Monitoring** - Real-time CPU percentage calculation using two-reading method from `/proc/stat`
- ✅ **Memory Usage Statistics** - Display total, used, and free memory (in MB) from `/proc/meminfo`
- ✅ **Top 5 Active Processes** - List processes consuming the most CPU resources with accurate calculation
- ✅ **Continuous Monitoring Mode** - Auto-refresh display at customizable intervals with live updates
- ✅ **Comprehensive Logging** - All operations logged to `syslog.txt` with timestamps
- ✅ **Signal Handling** - Graceful exit on Ctrl+C (SIGINT) in both menu and continuous modes
- ✅ **Error Handling** - Clear error messages using `perror()` for all system calls
- ✅ **Dual Interface** - Menu-driven and command-line modes
- ✅ **Help System** - Built-in help command (`-h`) for usage guidance

### Technical Implementation
- Uses Linux system calls (`open`, `read`, `close`, `opendir`, `readdir`, `closedir`)
- Reads from `/proc` filesystem (`/proc/stat`, `/proc/meminfo`, `/proc/[pid]/stat`, `/proc/[pid]/comm`)
- Two-reading method for accurate CPU usage calculation (1-second interval)
- Implements proper signal handling for SIGINT with `volatile sig_atomic_t` flag
- Input validation and comprehensive error checking
- Modular code structure with separate functions for each feature
- Memory values converted to MB for user-friendly display
- Screen clearing for continuous monitoring mode using ANSI escape codes

---

## 🛠️ System Requirements

- **Operating System:** Linux (Ubuntu, Debian, or WSL)
- **Compiler:** GCC (GNU Compiler Collection)
- **Libraries:** GNU glibc (standard C library)
- **Access:** `/proc` filesystem must be accessible
- **Permissions:** Read access to `/proc/stat`, `/proc/meminfo`, and `/proc/[pid]/` directories
- **Text Editor:** Any (vi, nano, gedit, VSCode)

---

## 📦 Installation & Compilation

### 1. Clone the Repository
```bash
git clone https://github.com/marylynkennedy12-cmyk/TMN4133-Group10-SysMonitor.git
cd TMN4133-Group10-SysMonitor
```

### 2. Compile the Program
```bash
gcc sysmonitor.c -o sysmonitor
```

**Expected Output:**
```
(No output means successful compilation)
```

**Note:** If you see warnings, they should be minor. The program has been tested and works correctly.

### 3. Verify Compilation
```bash
ls -l sysmonitor
```

You should see the executable file `sysmonitor` created with execute permissions.

---

## 🚀 Usage Guide

### Menu Mode (Interactive)

Run the program without arguments to enter menu mode:
```bash
./sysmonitor
```

**Menu Options:**
```
========================================
       SysMonitor++ Menu
========================================
1. CPU Usage
2. Memory Usage
3. Top 5 Processes
4. Continuous Monitoring
5. Exit
========================================
Enter choice:
```

Simply enter a number (1-5) to select an option.

**Features:**
- Input validation (rejects non-numeric input)
- Graceful error handling for invalid choices
- Ctrl+C returns to menu from continuous mode
- All actions are logged automatically

---

### Command-Line Mode (Non-Interactive)

#### Display Help
```bash
./sysmonitor -h
# or
./sysmonitor --help
```

**Output:**
```
SysMonitor++ - Linux System Resource Monitoring Tool
Usage:
  ./sysmonitor              Start interactive menu mode
  ./sysmonitor -m cpu       Display CPU usage
  ./sysmonitor -m mem       Display memory usage
  ./sysmonitor -m proc      List top 5 processes
  ./sysmonitor -c <seconds> Continuous monitoring mode
  ./sysmonitor -h           Show this help message
```

#### Display CPU Usage
```bash
./sysmonitor -m cpu
```

**Example Output:**
```
CPU Usage: 2.35%
```

**Technical Note:** Uses two-reading method with 1-second interval for accurate calculation. The program:
1. Reads CPU stats from `/proc/stat`
2. Waits 1 second
3. Reads CPU stats again
4. Calculates percentage based on delta values

#### Display Memory Usage
```bash
./sysmonitor -m mem
```

**Example Output:**
```
Memory: Total=7812 MB | Used=3072 MB | Free=4740 MB
```

**Note:** All values are displayed in MB (megabytes) for better readability. The calculation uses:
- **Total**: Total physical RAM from `MemTotal`
- **Used**: Total - MemAvailable
- **Free**: Available memory from `MemAvailable`

#### Display Top 5 Processes
```bash
./sysmonitor -m proc
```

**Example Output:**
```
Top 5 Active Processes:
PID        Name                           CPU%
------------------------------------------------------------
1234       chrome                         15.67%
5678       systemd                        2.34%
9012       bash                           0.89%
3456       sshd                           0.45%
7890       cron                           0.12%
```

**Technical Details:**
- Reads from `/proc/[pid]/stat` for CPU times
- Reads from `/proc/[pid]/comm` for process names
- Calculates CPU percentage based on process uptime
- Sorts by CPU usage in descending order
- Handles up to 512 processes (configurable via `MAX_PROCESSES`)

#### Continuous Monitoring
```bash
./sysmonitor -c 2
```

This refreshes the display every 2 seconds. Press **Ctrl+C** to exit gracefully.

**Display Format:**
```
=== SysMonitor++ Continuous Mode (Interval: 2 seconds) ===
Press Ctrl+C to exit.

CPU Usage: 1.23%

Memory: Total=7812 MB | Used=3072 MB | Free=4740 MB

Top 5 Active Processes:
PID        Name                           CPU%
------------------------------------------------------------
1234       chrome                         15.67%
5678       systemd                        2.34%
9012       bash                           0.89%
3456       sshd                           0.45%
7890       cron                           0.12%
```

**Features:**
- Screen clears between updates (using ANSI escape codes)
- Displays interval time in header
- Shows all three monitoring types together
- Graceful exit with Ctrl+C (returns to menu or exits)
- Invalid intervals default to 2 seconds
- All monitoring cycles are logged

---

## 📝 Logging System

All operations are automatically logged to `syslog.txt` with timestamps.

**Log Format:**
```
[YYYY-MM-DD HH:MM:SS] MODE=<mode>: <message>
```

**Example Log Entries:**
```
[2025-12-19 14:30:45] MODE=menu: Program started in menu mode
[2025-12-19 14:30:52] MODE=cpu: CPU Usage: 2.35%
[2025-12-19 14:31:02] MODE=mem: Memory: Total=7812 MB | Used=3072 MB | Free=4740 MB
[2025-12-19 14:31:15] MODE=proc: --- Top 5 Processes Start ---
[2025-12-19 14:31:15] MODE=proc: PID=1234 | chrome | CPU=15.67%
[2025-12-19 14:31:15] MODE=proc: PID=5678 | systemd | CPU=2.34%
[2025-12-19 14:31:15] MODE=proc: --- Top 5 Processes End ---
[2025-12-19 14:35:20] MODE=continuous: Continuous monitoring started
[2025-12-19 14:35:45] MODE=SIGINT: Session ended via Ctrl+C
[2025-12-19 14:35:45] MODE=continuous: Continuous monitoring stopped
[2025-12-19 14:40:10] MODE=menu: User selected Exit
```

**View Log File:**
```bash
cat syslog.txt
# or for live updates
tail -f syslog.txt
# or with editor
nano syslog.txt
```

**Log Features:**
- Append-only (never overwrites previous logs)
- Timestamps use local system time
- Each monitoring operation is logged
- Start and end markers for process lists
- SIGINT events are captured
- Error messages are logged via `perror()`

---

## 🎮 Signal Handling

The program handles **SIGINT** (Ctrl+C) gracefully using the `signal()` system call and a `volatile sig_atomic_t` flag:

**Behavior:**
- **During Continuous Monitoring:** Sets stop flag, exits monitoring loop, logs event, returns to menu (if started from menu) or exits (if started from command line)
- **From Menu:** Exits program after logging the event
- **Logs the event** with timestamp and mode information

**Example:**
```bash
# In continuous mode
./sysmonitor -c 2
# Press Ctrl+C
^C
Exiting... Saving log.

# Log shows:
[2025-12-19 14:35:45] MODE=SIGINT: Session ended via Ctrl+C
[2025-12-19 14:35:45] MODE=continuous: Continuous monitoring stopped
```

**Technical Implementation:**
- Uses `signal(SIGINT, handleSignal)` to register handler
- Handler sets `volatile sig_atomic_t stop_flag = 1`
- Main loop checks flag every 0.1 seconds during sleep intervals
- Safe for concurrent execution (atomic flag operations)

---

## 📁 Project Structure
```
TMN4133-Group10-SysMonitor/
│
├── sysmonitor.c          # Main source code (final version)
├── sysmonitor            # Compiled executable (after compilation)
├── syslog.txt            # Log file (auto-generated during runtime)
├── README.md             # This file (project documentation)
├── .gitignore            # Git ignore rules (excludes binaries and logs)
└── report.pdf            # Project report (for submission)
```

---

## 👥 Team Members

**Group 10 - Division of Responsibilities**

| Member | Name | Role | Tasks |
|--------|------|------|-------|
| **Member 1** | Marylyn anak Kennedy | Team Leader & Core Programmer | - Main Menu System<br>- Continuous Monitoring Mode<br>- Signal Handling (SIGINT)<br>- Module Integration<br>- GitHub Repository Setup<br>- README.md Creation |
| **Member 2** | Joycelyn Chan Mei Tong | CPU/Memory Programmer & Testing Lead | - CPU Usage Function (`getCPUUsage()` with two-reading method)<br>- Memory Usage Function (`getMemoryUsage()` with MB conversion)<br>- Compilation Tests<br>- Menu & CLI Testing<br>- Test Summary Table |
| **Member 3** | Alexandrea Abegail Severius Moikon | Processes, Logging & Error Handling | - Top 5 Processes (`listTopProcesses()` with accurate calculation)<br>- Logging System (`write_log()`)<br>- Error Handling with `perror()`<br>- CLI & Error Case Testing<br>- Log Verification |
| **Member 4** | Simi Marceline Ulau Daud | Documentation & Presentation Lead | - Task C: AI Reflection<br>- Task D: GitHub Documentation<br>- Task E: Presentation Preparation<br>- Video Editing & Upload<br>- Slide Preparation |

---

## 🧪 Testing

The program has been thoroughly tested for:

✅ **Compilation** - No errors, only minor warnings (if any)  
✅ **Menu Mode** - All 5 options functional with input validation  
✅ **CLI Mode** - All flags (`-m cpu/mem/proc`, `-c`, `-h`) working correctly  
✅ **Signal Handling** - Ctrl+C handled gracefully in all modes  
✅ **Error Cases** - Invalid inputs, missing arguments, invalid intervals  
✅ **Logging** - All operations logged with correct timestamps  
✅ **Edge Cases** - Empty /proc directories, permission errors, process limit  
✅ **CPU Calculation** - Two-reading method provides accurate percentages  
✅ **Memory Display** - Values correctly converted to MB  
✅ **Process List** - Accurate CPU calculation per process  

Detailed test results and screenshots are available in the project report.

---

## 🛠️ Known Issues & Notes

### CPU Usage Shows Low Percentages (0-2%)
This is **normal behavior** when the system is idle, especially in WSL environments:
- Modern CPUs are very efficient and spend most time idle
- WSL virtualizes the Linux kernel, reducing apparent CPU activity
- Browser activity (YouTube, etc.) uses Windows GPU, not visible to WSL
- Background processes are highly optimized

**To verify the tool works correctly:**
```bash
# Generate CPU load
yes > /dev/null &

# Check CPU usage (should show higher percentage)
./sysmonitor -m cpu

# Kill the load generator
killall yes
```

### Memory Values in MB
- Memory values are displayed in **MB (megabytes)** for better readability
- Original values from `/proc/meminfo` are in kB (kilobytes)
- Conversion: `MB = kB / 1024`
- "Used" memory = Total - Available (following Linux memory management convention)

### Process Limit
- Maximum of 512 processes can be monitored (configurable via `MAX_PROCESSES` constant)
- If system has more processes, a warning is displayed
- Top 5 are still calculated correctly from the first 512 processes

### Screen Clearing in Continuous Mode
- Uses ANSI escape codes (`\033[H\033[J`)
- May not work in all terminal emulators
- If screen doesn't clear properly, output will still be correct (just not cleared)

---

## 🔧 Troubleshooting

### Problem: "Permission denied" when reading /proc
**Solution:** 
- Check file permissions: `ls -l /proc/stat /proc/meminfo`
- Most /proc files should be world-readable
- Check SELinux/AppArmor settings if on a hardened system
- Try running from a different directory

### Problem: Compilation warnings about unused variables
**Solution:** 
- These are minor and don't affect functionality
- The code includes `(void)sig;` to suppress parameter warnings
- You can ignore these warnings safely

### Problem: Log file not created
**Solution:** 
- Check write permissions in current directory: `ls -ld .`
- Try creating the file manually: `touch syslog.txt`
- Ensure disk space is available: `df -h .`

### Problem: CPU percentage always 0.00%
**Solution:**
- This happens on extremely idle systems
- Generate some CPU load to test (see "Known Issues" section)
- The calculation is correct; system is just truly idle
- Try running during active usage

### Problem: Continuous monitoring doesn't respond to Ctrl+C immediately
**Solution:**
- The program checks the stop flag every 0.1 seconds
- Maximum delay is your refresh interval
- This is normal behavior; wait up to 1 interval cycle
- The exit will be logged correctly

### Problem: "Invalid input" in menu mode
**Solution:**
- Only enter numbers 1-5
- Avoid letters, symbols, or special characters
- Press Enter after typing your choice
- The buffer is cleared automatically on invalid input

---

## 📚 Technical Details

### Functions Overview

| Function | Purpose | System Calls Used | File Operations |
|----------|---------|-------------------|-----------------|
| `read_proc_stat()` | Read CPU statistics | `open()`, `read()`, `close()` | Reads `/proc/stat` |
| `getCPUPercentage()` | Calculate CPU % using two-reading method | Calls `read_proc_stat()` twice | Two readings 1 second apart |
| `getCPUUsage()` | Display & log CPU usage | Calls `getCPUPercentage()` | Logs to `syslog.txt` |
| `getMemoryUsage()` | Display & log memory stats | `open()`, `read()`, `close()` | Reads `/proc/meminfo`, converts to MB |
| `readProcCPU()` | Calculate single process CPU | `fopen()`, `fscanf()`, `fclose()` | Reads `/proc/[pid]/stat` and `/proc/uptime` |
| `listTopProcesses()` | Show top 5 CPU consumers | `opendir()`, `readdir()`, `closedir()`, `fopen()` | Reads `/proc/[pid]/stat` & `/proc/[pid]/comm` |
| `continuousMonitor()` | Auto-refresh monitoring | Calls above functions, `usleep()` | Checks stop flag every 0.1s |
| `handleSignal()` | Handle Ctrl+C gracefully | `signal()` | Sets stop flag, logs SIGINT event |
| `write_log()` | Append to log file | `fopen()`, `fprintf()`, `fclose()` | Writes to `syslog.txt` |
| `showHelp()` | Display help message | `printf()` | Terminal output only |
| `showMenu()` | Interactive menu loop | `scanf()`, `printf()` | Handles all menu operations |

### System Calls & Library Functions Used

**File I/O:**
- `open()` / `close()` - Low-level file operations for /proc files
- `read()` - Read raw data from file descriptors
- `fopen()` / `fclose()` - High-level file stream operations
- `fgets()` / `fscanf()` - Formatted reading from streams
- `fprintf()` - Formatted writing to log file

**Directory Operations:**
- `opendir()` - Open directory stream
- `readdir()` - Read directory entries
- `closedir()` - Close directory stream

**Signal Handling:**
- `signal()` - Register signal handler
- `volatile sig_atomic_t` - Atomic flag for signal safety

**Time & String Operations:**
- `time()` / `localtime()` / `strftime()` - Timestamp generation
- `sprintf()` / `snprintf()` - Safe string formatting
- `strcmp()` / `strstr()` - String comparison and search
- `strcspn()` - String span operations

**System Information:**
- `sysconf(_SC_CLK_TCK)` - Get system clock ticks per second
- `sleep()` - Sleep for seconds (used in CPU calculation)
- `usleep()` - Microsecond sleep (used in continuous monitoring)

**Error Handling:**
- `perror()` - Print system error messages

### CPU Usage Calculation Algorithm

The program uses the **two-reading method** for accurate CPU usage calculation:

```c
// Pseudo-code algorithm
1. Read /proc/stat → Get CPU time counters (user, nice, system, idle, iowait, irq, softirq)
2. Calculate: total1 = sum of all counters, idle1 = idle + iowait
3. Sleep for 1 second
4. Read /proc/stat again → Get new counters
5. Calculate: total2 = sum of all counters, idle2 = idle + iowait
6. Calculate deltas: totald = total2 - total1, idled = idle2 - idle1
7. CPU% = 100 * (totald - idled) / totald
```

**Why two readings?**
- Single reading only shows cumulative CPU time since boot
- Two readings allow calculation of CPU usage in the interval
- More accurate than instantaneous sampling

### Process CPU Calculation

Per-process CPU usage calculation:

```c
// Pseudo-code
1. Read /proc/[pid]/stat → Get process CPU times (utime, stime, starttime)
2. Read /proc/uptime → Get system uptime
3. total_time = utime + stime (in clock ticks)
4. process_seconds = uptime - (starttime / ticks_per_second)
5. CPU% = 100 * (total_time / ticks_per_second) / process_seconds
```

**Note:** This calculates average CPU usage since process start, not instantaneous usage.

---

## 📖 Additional Resources

- [Linux /proc Filesystem Documentation](https://www.kernel.org/doc/html/latest/filesystems/proc.html)
- [GCC Compiler Manual](https://gcc.gnu.org/onlinedocs/)
- [Signal Handling in C](https://man7.org/linux/man-pages/man7/signal.7.html)
- [Linux System Programming](https://man7.org/linux/man-pages/)

---

## 📄 License

This project is created for educational purposes as part of TMN4133 System Programming course requirements at Universiti Malaysia Sarawak.

**Academic Integrity Notice:** This code is submitted as coursework. Please do not copy or reuse without proper attribution and instructor permission.

---

## 🙏 Acknowledgments

- **Lecturer:** Johari bin Abdullah - For guidance and support throughout the project
- **Course:** TMN4133 System Programming
- **Institution:** Universiti Malaysia Sarawak
- **Tools Used:** 
  - GCC Compiler
  - GitHub for version control
  - WSL (Windows Subsystem for Linux) / Ubuntu
  - AI Assistants (Claude, ChatGPT, Gemini) for debugging and learning
  - nano for code editing

---

## 🔗 GitHub Repository

**Group 10**  
Repository: [https://github.com/marylynkennedy12-cmyk/TMN4133-Group10-SysMonitor](https://github.com/marylynkennedy12-cmyk/TMN4133-Group10-SysMonitor)

---

## 📅 Project Timeline

- **Week 6:** Project assigned, repository created, initial planning
- **Week 7-8:** Core functionality development (CPU, Memory, Processes)
- **Week 9:** Testing, debugging, and signal handling implementation
- **Week 10:** Documentation, logging system, and presentation preparation
- **Week 11:** Final testing, README updates, and submission (December 19, 2025)

---

**Last Updated:** December 19, 2025  
**Version:** 2.0 (Final Release)  
**Status:** ✅ Complete and Ready for Submission

---

## 🎯 Quick Start Guide

**For first-time users:**

```bash
# 1. Clone and compile
git clone https://github.com/marylynkennedy12-cmyk/TMN4133-Group10-SysMonitor.git
cd TMN4133-Group10-SysMonitor
gcc sysmonitor.c -o sysmonitor

# 2. Try menu mode
./sysmonitor

# 3. Try command-line mode
./sysmonitor -m cpu
./sysmonitor -m mem
./sysmonitor -m proc

# 4. Try continuous monitoring
./sysmonitor -c 3

# 5. Check the log
cat syslog.txt
```

**Enjoy monitoring your system! 🖥️📊**