#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#define LOGFILE "syslog.txt"
#define MAX_PROCESSES 512

volatile sig_atomic_t stop_flag = 0;

/* ---------- Logging ---------- */
void write_log(const char *mode, const char *msg) {
    FILE *f = fopen(LOGFILE, "a");
    if (!f) {
        perror("Error opening log file");
        return;
    }

    time_t t = time(NULL);
    char ts[64];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&t));
    fprintf(f, "[%s] MODE=%s: %s\n", ts, mode, msg);
    fclose(f);
}

/* ---------- CPU USAGE (Two-Reading Method) ---------- */
int read_proc_stat(unsigned long long *user, unsigned long long *nicev, 
                   unsigned long long *sys, unsigned long long *idle, 
                   unsigned long long *iowait, unsigned long long *irq, 
                   unsigned long long *softirq) {
    
    int fd = open("/proc/stat", O_RDONLY);
    if (fd == -1) {
        perror("Error opening /proc/stat");
        return -1;
    }

    char buf[1024];
    int bytes_read = read(fd, buf, sizeof(buf) - 1);
    if (bytes_read == -1) {
        perror("Error reading /proc/stat");
        close(fd);
        return -1;
    }
    buf[bytes_read] = '\0';
    close(fd);

    // Parse first line: cpu user nice system idle iowait irq softirq
    sscanf(buf, "cpu %llu %llu %llu %llu %llu %llu %llu",
           user, nicev, sys, idle, iowait, irq, softirq);
    return 0;
}

double getCPUPercentage() {
    unsigned long long user, nicev, sys, idle, iowait, irq, softirq;
    unsigned long long total1, idle1, total2, idle2;

    // First reading
    if (read_proc_stat(&user, &nicev, &sys, &idle, &iowait, &irq, &softirq) == -1) 
        return 0.0;
    
    idle1 = idle + iowait;
    total1 = user + nicev + sys + idle + iowait + irq + softirq;

    sleep(1); // Wait 1 second between readings

    // Second reading
    if (read_proc_stat(&user, &nicev, &sys, &idle, &iowait, &irq, &softirq) == -1) 
        return 0.0;

    idle2 = idle + iowait;
    total2 = user + nicev + sys + idle + iowait + irq + softirq;

    // Calculate delta
    double totald = (double)(total2 - total1);
    double idled = (double)(idle2 - idle1);

    if (totald == 0) return 0.0;
    return (100.0 * (totald - idled) / totald);
}

void getCPUUsage() {
    double cpu = getCPUPercentage();
    char msg[128];
    snprintf(msg, sizeof(msg), "CPU Usage: %.2f%%", cpu);
    printf("%s\n", msg);
    write_log("cpu", msg);
}

/* ---------- MEMORY USAGE (Converted to MB) ---------- */
void getMemoryUsage() {
    int fd = open("/proc/meminfo", O_RDONLY);
    if (fd == -1) {
        perror("Error opening /proc/meminfo");
        return;
    }

    char buf[2048];
    int bytes = read(fd, buf, sizeof(buf) - 1);
    close(fd);

    if (bytes <= 0) {
        perror("Error reading /proc/meminfo");
        return;
    }
    buf[bytes] = '\0';

    long total = 0, available = 0;
    
    // Parse buffer for MemTotal and MemAvailable
    char *total_ptr = strstr(buf, "MemTotal:");
    if (total_ptr) sscanf(total_ptr, "MemTotal: %ld kB", &total);
    
    char *avail_ptr = strstr(buf, "MemAvailable:");
    if (avail_ptr) sscanf(avail_ptr, "MemAvailable: %ld kB", &available);

    long used = total - available;
    
    // Convert to MB for user-friendly display
    char msg[256];
    snprintf(msg, sizeof(msg), 
             "Memory: Total=%ld MB | Used=%ld MB | Free=%ld MB", 
             total/1024, used/1024, available/1024);
    printf("%s\n", msg);
    write_log("mem", msg);
}

/* ---------- TOP PROCESSES ---------- */
typedef struct {
    int pid;
    char name[256];
    double cpu;
} ProcInfo;

double readProcCPU(int pid) {
    char path[64];
    sprintf(path, "/proc/%d/stat", pid);

    FILE *f = fopen(path, "r");
    if (!f) return 0.0;

    long utime, stime;
    long long starttime;
    int dummy_pid;
    char comm[256];

    // Read process uptime
    FILE *fu = fopen("/proc/uptime", "r");
    if (!fu) {
        fclose(f);
        return 0.0;
    }

    double uptime;
    fscanf(fu, "%lf", &uptime);
    fclose(fu);

    // Parse /proc/[pid]/stat
    fscanf(f, "%d %s", &dummy_pid, comm);
    for (int x = 0; x < 11; x++) fscanf(f, "%*s");
    fscanf(f, "%ld %ld", &utime, &stime);
    for (int x = 0; x < 4; x++) fscanf(f, "%*s");
    fscanf(f, "%lld", &starttime);
    fclose(f);

    long ticks = sysconf(_SC_CLK_TCK);
    if (ticks <= 0) return 0.0;

    double total_time = (double)(utime + stime);
    double seconds = uptime - ((double)starttime / ticks);

    if (seconds <= 0) return 0.0;
    return 100.0 * ((total_time / ticks) / seconds);
}

void listTopProcesses() {
    DIR *d = opendir("/proc");
    if (!d) {
        perror("Error opening /proc");
        return;
    }

    struct dirent *ent;
    ProcInfo list[MAX_PROCESSES];
    int count = 0;

    while ((ent = readdir(d))) {
        if (!isdigit(ent->d_name[0])) continue;

        int pid = atoi(ent->d_name);
        char commPath[64];
        sprintf(commPath, "/proc/%d/comm", pid);

        FILE *f = fopen(commPath, "r");
        if (!f) continue;

        fgets(list[count].name, sizeof(list[count].name), f);
        list[count].name[strcspn(list[count].name, "\n")] = 0;
        fclose(f);
        
        list[count].pid = pid;
        list[count].cpu = readProcCPU(pid);
        count++;
        
        if (count >= MAX_PROCESSES) {
            fprintf(stderr, "Warning: Process limit (%d) reached. Showing first %d processes.\n", 
                    MAX_PROCESSES, MAX_PROCESSES);
            break;
        }
    }
    closedir(d);

    // Sort by CPU usage (descending)
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (list[j].cpu > list[i].cpu) {
                ProcInfo tmp = list[i];
                list[i] = list[j];
                list[j] = tmp;
            }
        }
    }

    printf("Top 5 Active Processes:\n");
    printf("%-10s %-30s %s\n", "PID", "Name", "CPU%%");
    printf("------------------------------------------------------------\n");
    
    write_log("proc", "--- Top 5 Processes Start ---");

    int display_count = (count < 5) ? count : 5;
    for (int i = 0; i < display_count; i++) {
        char msg[512];
        snprintf(msg, sizeof(msg), "PID=%d | %s | CPU=%.2f%%",
                 list[i].pid, list[i].name, list[i].cpu);
        printf("%-10d %-30s %.2f%%\n", list[i].pid, list[i].name, list[i].cpu);
        write_log("proc", msg);
    }
    
    write_log("proc", "--- Top 5 Processes End ---");
}

/* ---------- CONTINUOUS MONITORING ---------- */
void continuousMonitor(int interval) {
    if (interval <= 0) {
        printf("Invalid interval. Using default 2 seconds.\n");
        interval = 2;
    }

    write_log("continuous", "Continuous monitoring started");
    
    while (1) {
        if (stop_flag) break;
        
        printf("\033[H\033[J"); // Clear screen
        printf("=== SysMonitor++ Continuous Mode (Interval: %d seconds) ===\n", interval);
        printf("Press Ctrl+C to exit.\n\n");

        getCPUUsage();
        printf("\n");
        getMemoryUsage();
        printf("\n");
        listTopProcesses();
        printf("\n");

        // Check stop flag every 0.1 seconds
        for (int i = 0; i < interval * 10; i++) {
            if (stop_flag) break;
            usleep(100000);
        }

        if (stop_flag) break;
    }
    
    write_log("continuous", "Continuous monitoring stopped");
}

/* ---------- SIGNAL HANDLER ---------- */
void handleSignal(int sig) {
    (void)sig; // Suppress unused parameter warning
    stop_flag = 1;
    write_log("SIGINT", "Session ended via Ctrl+C");
    printf("\nExiting... Saving log.\n");
}

/* ---------- HELP DISPLAY ---------- */
void showHelp() {
    printf("SysMonitor++ - Linux System Resource Monitoring Tool\n");
    printf("Usage:\n");
    printf("  ./sysmonitor              Start interactive menu mode\n");
    printf("  ./sysmonitor -m cpu       Display CPU usage\n");
    printf("  ./sysmonitor -m mem       Display memory usage\n");
    printf("  ./sysmonitor -m proc      List top 5 processes\n");
    printf("  ./sysmonitor -c <seconds> Continuous monitoring mode\n");
    printf("  ./sysmonitor -h           Show this help message\n");
}

/* ---------- MENU ---------- */
void showMenu() {
    int choice;
    while (1) {
        stop_flag = 0;
        printf("\n========================================\n");
        printf("       SysMonitor++ Menu\n");
        printf("========================================\n");
        printf("1. CPU Usage\n");
        printf("2. Memory Usage\n");
        printf("3. Top 5 Processes\n");
        printf("4. Continuous Monitoring\n");
        printf("5. Exit\n");
        printf("========================================\n");
        printf("Enter choice: ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input. Please enter a number (1-5).\n");
            continue;
        }

        if (choice == 1) {
            getCPUUsage();
        }
        else if (choice == 2) {
            getMemoryUsage();
        }
        else if (choice == 3) {
            listTopProcesses();
        }
        else if (choice == 4) {
            int sec;
            printf("Enter refresh interval (seconds): ");
            if (scanf("%d", &sec) != 1 || sec <= 0) {
                printf("Invalid interval. Using default 2 seconds.\n");
                sec = 2;
                while (getchar() != '\n');
            }
            continuousMonitor(sec);
        }
        else if (choice == 5) {
            write_log("menu", "User selected Exit");
            printf("Goodbye!\n");
            return;
        }
        else {
            printf("Invalid choice. Please select 1-5.\n");
        }
    }
}

/* ---------- MAIN ---------- */
int main(int argc, char *argv[]) {
    signal(SIGINT, handleSignal);

    // Command-line mode
    if (argc > 1) {
        // Help option
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            showHelp();
            return 0;
        }

        // Monitor mode (-m)
        if (strcmp(argv[1], "-m") == 0) {
            if (argc < 3) {
                printf("Error: missing parameter. Use -m [cpu|mem|proc]\n");
                return 1;
            }
            if (strcmp(argv[2], "cpu") == 0) {
                getCPUUsage();
            }
            else if (strcmp(argv[2], "mem") == 0) {
                getMemoryUsage();
            }
            else if (strcmp(argv[2], "proc") == 0) {
                listTopProcesses();
            }
            else {
                printf("Invalid argument for -m. Use: cpu, mem, or proc.\n");
                return 1;
            }
            return 0;
        }

        // Continuous mode (-c)
        if (strcmp(argv[1], "-c") == 0) {
            if (argc < 3) {
                printf("Error: missing parameter. Use -c <seconds>\n");
                return 1;
            }
            int sec = atoi(argv[2]);
            if (sec <= 0) {
                printf("Invalid interval. Using default 2 seconds.\n");
                sec = 2;
            }
            continuousMonitor(sec);
            return 0;
        }

        // Invalid option
        printf("Invalid option: %s\n", argv[1]);
        printf("Use -h for help.\n");
        return 1;
    }

    // No arguments - start menu mode
    write_log("menu", "Program started in menu mode");
    showMenu();
    return 0;
}

