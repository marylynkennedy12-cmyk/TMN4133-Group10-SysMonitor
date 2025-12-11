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
- ✅ **CPU Usage Monitoring** - Real-time CPU percentage calculation from `/proc/stat`
- ✅ **Memory Usage Statistics** - Display total, used, and available memory from `/proc/meminfo`
- ✅ **Top 5 Active Processes** - List processes consuming the most CPU resources
- ✅ **Continuous Monitoring Mode** - Auto-refresh display at customizable intervals
- ✅ **Comprehensive Logging** - All operations logged to `syslog.txt` with timestamps
- ✅ **Signal Handling** - Graceful exit on Ctrl+C (SIGINT)
- ✅ **Error Handling** - Clear error messages using `perror()`
- ✅ **Dual Interface** - Menu-driven and command-line modes

### Technical Implementation
- Uses Linux system calls (`open`, `read`, `close`)
- Reads from `/proc` filesystem
- Implements proper signal handling for SIGINT
- Input validation and error checking
- Modular code structure with separate functions

---

## 🛠️ System Requirements

- **Operating System:** Linux (Ubuntu or WSL)
- **Compiler:** GCC (GNU Compiler Collection)
- **Libraries:** GNU glibc
- **Access:** `/proc` filesystem must be accessible
- **Text Editor:** Any (vi, nano, gedit)

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

### 3. Verify Compilation
```bash
ls -l sysmonitor
```

You should see the executable file `sysmonitor` created.

---

## 🚀 Usage Guide

### Menu Mode (Interactive)

Run the program without arguments to enter menu mode:
```bash
./sysmonitor
```

**Menu Options:**
```
SysMonitor++ Menu
1. CPU Usage
2. Memory Usage
3. Top 5 Processes
4. Continuous Monitoring
5. Exit
Enter choice:
```

Simply enter a number (1-5) to select an option.

---

### Command-Line Mode (Non-Interactive)

#### Display CPU Usage
```bash
./sysmonitor -m cpu
```

**Example Output:**
```
CPU=2.35%
```

#### Display Memory Usage
```bash
./sysmonitor -m mem
```

**Example Output:**
```
Memory Used=3145728 kB / 8000000 kB
```

#### Display Top 5 Processes
```bash
./sysmonitor -m proc
```

**Example Output:**
```
Top 5 Active Processes:
PID=1234 | chrome | CPU=15.67%
PID=5678 | systemd | CPU=2.34%
PID=9012 | bash | CPU=0.89%
PID=3456 | sshd | CPU=0.45%
PID=7890 | cron | CPU=0.12%
```

#### Continuous Monitoring
```bash
./sysmonitor -c 2
```

This refreshes the display every 2 seconds. Press **Ctrl+C** to exit and return to the menu.

**Display shows:**
```
Press Ctrl + C to return to menu.

CPU=1.23%
Memory Used=3145728 kB / 8000000 kB
Top 5 Active Processes:
PID=1234 | chrome | CPU=15.67%
...
```

---

## 📝 Logging System

All operations are automatically logged to `syslog.txt` with timestamps.

**Log Format:**
```
[YYYY-MM-DD HH:MM:SS] MODE=<mode>: <message>
```

**Example Log Entries:**
```
[2025-12-11 14:30:45] MODE=cpu: CPU=2.35%
[2025-12-11 14:31:02] MODE=mem: Memory Used=3145728 kB / 8000000 kB
[2025-12-11 14:31:15] MODE=proc: PID=1234 | chrome | CPU=15.67%
[2025-12-11 14:35:20] MODE=SIGINT: Session ended via Ctrl+C
[2025-12-11 14:40:10] MODE=menu: User selected Exit
```

**View Log File:**
```bash
cat syslog.txt
# or
nano syslog.txt
```

---

## 🎮 Signal Handling

The program handles **SIGINT** (Ctrl+C) gracefully:

- **During Continuous Monitoring:** Returns to main menu
- **From Menu:** Exits program after saving logs
- **Logs the event** with timestamp

**Example:**
```
^C
Exiting... Log saved.
```

---

## 📁 Project Structure
```
TMN4133-Group10-SysMonitor/
│
├── sysmonitor.c          # Main source code
├── sysmonitor            # Compiled executable (after compilation)
├── syslog.txt            # Log file (auto-generated)
├── README.md             # This file
├── .gitignore            # Git ignore rules
└── report.pdf            # Project report (for submission)
```

---

## 👥 Team Members

**Group 10 - Division of Responsibilities**

| Member | Name | Role | Tasks |
|--------|------|------|-------|
| **Member 1** | Marylyn anak Kennedy | Team Leader & Core Programmer | - Main Menu System<br>- Continuous Monitoring Mode<br>- Signal Handling (SIGINT)<br>- Module Integration<br>- GitHub Repository Setup<br>- README.md Creation |
| **Member 2** | Joycelyn Chan Mei Tong | CPU/Memory Programmer & Testing Lead | - CPU Usage Function (`getCPUUsage()`)<br>- Memory Usage Function (`getMemoryUsage()`)<br>- Compilation Tests<br>- Menu & CLI Testing<br>- Test Summary Table |
| **Member 3** | Alexandrea Abegail Severius Moikon | Processes, Logging & Error Handling | - Top 5 Processes (`listTopProcesses()`)<br>- Logging System (`write_log()`)<br>- Error Handling with `perror()`<br>- CLI & Error Case Testing<br>- Log Verification |
| **Member 4** | Simi Marceline Ulau Daud | Reflection, GitHub & Presentation Lead | - Task C: AI Reflection<br>- Task D: GitHub Documentation<br>- Task E: Presentation Preparation<br>- Video Editing & Upload<br>- Slide Preparation |

---

## 🧪 Testing

The program has been thoroughly tested for:

✅ **Compilation** - No warnings or errors  
✅ **Menu Mode** - All 5 options functional  
✅ **CLI Mode** - All flags (`-m`, `-c`) working  
✅ **Signal Handling** - Ctrl+C handled gracefully  
✅ **Error Cases** - Invalid inputs, missing arguments  
✅ **Logging** - All operations logged with timestamps  
✅ **Edge Cases** - Empty /proc, permission errors  

Detailed test results and screenshots are available in the project report.

---

## 🐛 Known Issues & Notes

### CPU Usage Shows 0.00%
This is **normal behavior** when the system is idle, especially in WSL environments:
- Modern CPUs are very efficient and spend most time idle
- WSL virtualizes the Linux kernel, reducing apparent CPU activity
- Browser activity (YouTube, etc.) uses Windows GPU, not visible to WSL
- To verify the tool works: run `yes > /dev/null &` to generate CPU load

### Memory Values
- Memory values are in kilobytes (kB) as reported by `/proc/meminfo`
- "Used" memory = Total - Available (following Linux memory management)

---

## 🔧 Troubleshooting

### Problem: "Permission denied" when reading /proc
**Solution:** Run with appropriate permissions or check SELinux/AppArmor settings

### Problem: Compilation warnings
**Solution:** Ensure you're using GCC and have all required headers

### Problem: Log file not created
**Solution:** Check write permissions in the current directory

### Problem: Continuous monitoring doesn't exit
**Solution:** Press Ctrl+C to trigger SIGINT handler

---

## 📚 Technical Details

### Functions Overview

| Function | Purpose | File Operations |
|----------|---------|-----------------|
| `getCPUUsage()` | Calculate & display CPU % | Reads `/proc/stat` |
| `getMemoryUsage()` | Display memory statistics | Reads `/proc/meminfo` |
| `listTopProcesses()` | Show top 5 CPU consumers | Reads `/proc/[pid]/stat` & `/proc/[pid]/comm` |
| `continuousMonitor()` | Auto-refresh monitoring | Calls above functions periodically |
| `handleSignal()` | Handle Ctrl+C gracefully | Logs exit event |
| `write_log()` | Append to log file | Writes to `syslog.txt` |

### System Calls Used
- `fopen()` / `fclose()` - File operations
- `fgets()` / `fscanf()` - Reading /proc files
- `opendir()` / `readdir()` / `closedir()` - Directory traversal
- `signal()` - Signal handler registration
- `perror()` - Error message display

---

## 📖 Additional Resources

- [Linux /proc Filesystem Documentation](https://www.kernel.org/doc/html/latest/filesystems/proc.html)
- [GCC Compiler Manual](https://gcc.gnu.org/onlinedocs/)
- [Signal Handling in C](https://man7.org/linux/man-pages/man7/signal.7.html)

---

## 📄 License

This project is created for educational purposes as part of TMN4133 System Programming course requirements UNIMAS.

**Academic Integrity Notice:** This code is submitted as coursework. Please do not copy or reuse without proper attribution.

---

## 🙏 Acknowledgments

- **Lecturer:** Johari bin Abdullah - For guidance and support
- **Course:** TMN4133 System Programming
- **Tools Used:** 
  - GCC Compiler
  - GitHub for version control
  - WSL (Windows Subsystem for Linux) / Ubuntu
  - AI Assistants (Claude, ChatGPT, Gemini) for debugging and learning
  - nano/vim for code editing

---

## Link Github

**Group 10**  
- GitHub: [https://github.com/marylynkennedy12-cmyk/TMN4133-Group10-SysMonitor](https://github.com/marylynkennedy12-cmyk/TMN4133-Group10-SysMonitor)

---

## 📅 Project Timeline

- **Week 6:** Project assigned, repository created
- **Week 7-8:** Core functionality development
- **Week 9:** Testing and debugging
- **Week 10:** Documentation and presentation preparation
- **Week 11:** Final submission (December 19, 2025)

---

**Last Updated:** December 11, 2025  
**Version:** 1.0  
**Status:** ✅ Complete and Ready for Submission
