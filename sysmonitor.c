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

volatile sig_atomic_t stop_flag = 0;

//Implement write_log-Member 3
void write_log(const char *mode, const char *msg)
{
	FILE *f = fopen(LOGFILE, "a");
	if (!f) return;

	time_t t = time(NULL);
	char ts[64];
	strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&t));
	fprintf(f, "[%s] MODE=%s: %s\n", ts, mode, msg);
	fclose(f);
	fprintf(stderr, "LOGGING: %s\n", msg);
}

//Implement listTopProcess- Member 3
typedef struct
{
	int pid;
	char name[256];
	double cpu;
} ProcInfo;

double readProcCPU(int pid)
{
	char path[64];
	sprintf(path,"/proc/%d/stat", pid);

	FILE *f = fopen(path, "r");
	if (!f) return 0;
	
	long utime, stime;
	long long starttime;
	int i;

	FILE *fu = fopen("/proc/uptime", "r");
	if (!fu)
	{
		fclose(f);
		return 0;
	}

	long long uptime;
	fscanf(fu, "%lld", &uptime);
	fclose(fu);

	char comm[256];
        fscanf(f, "%d %s", &i, comm);
        for (int x = 0; x < 11; x++) fscanf(f, "%*s"); // skip fields

        fscanf(f, "%ld %ld", &utime, &stime); // fields 14 & 15
        for (int x = 0; x < 4; x++) fscanf(f, "%*s");
        fscanf(f, "%lld", &starttime);           // field 22
        fclose(f);

        double total_time = utime + stime;
        double seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));

        if (seconds <= 0) return 0;
        return 100.0 * ((total_time / sysconf(_SC_CLK_TCK)) / seconds);
}

void listTopProcesses()
{
	DIR *d = opendir("/proc");
	if (!d)
	{
		perror("Error opening /proc");
		return;
	}

	struct dirent *ent;
	ProcInfo list[512];
	int count = 0;

	while ((ent = readdir(d)))
	{
		if (!isdigit(ent -> d_name[0])) continue;

		int pid = atoi(ent -> d_name);
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
		if (count >= 512) break;
	}
	closedir(d);

	//Sort by CPU usage desc
	for (int i = 0; i < count; i++)
	{
		for (int j = i+1; j < count; j++)
		{
			if (list[j].cpu > list[i].cpu)
			{
				ProcInfo tmp = list[i];
				list[i] = list[j];
				list[j] = tmp;
			}
		}
	}

	printf("Top 5 Active Processes: \n");
	char msg[512];
	//Header for log clarity
	write_log("proc", "--- Top 5 processes start ---");
	
	for (int i = 0; i < 5 && i < count; i++)
	{
		snprintf(msg, sizeof(msg), "PID=%d | %s | CPU=%.2f%%", list[i].pid, list[i].name, list[i].cpu);
		printf("%s\n", msg);
		write_log("proc", msg);
	}
}

//Implement Continuous Monitoring Mode-Member 1
void continuousMonitor(int interval) {
    write_log("continuous", "Continuous monitoring started");
    while (1) {
        if (stop_flag) break;
        printf("\033[H\033[J"); // clear screen

        printf("Press Ctrl + C to exit continuous mode.\n\n");

        // Calls to other modules (currently placeholders)
        getCPUUsage(); 
        getMemoryUsage();
        listTopProcesses();

        for (int i = 0; i < interval * 10; i++) {
            if (stop_flag) break;
            usleep(100000); // check every 0.1s
        }

        if (stop_flag) break;
    }
    write_log("continuous", "Continuous monitoring stopped");
}

//Implement Signal Handling (SIGINT)-Member 1
void handleSignal(int sig) {
    stop_flag = 1;
    // Log the session end via Ctrl+C, as required by spec.
    write_log("SIGINT", "Session ended via Ctrl+C");
    printf("\nExiting... Saving log.\n");
}

//Implement Main Menu System-Member 1
void showMenu() {
    int choice;
    while (1) {
        stop_flag = 0; // reset flag when returning to menu
        printf("\nSysMonitor++ Menu\n");
        printf("1. CPU Usage\n");
        printf("2. Memory Usage\n");
        printf("3. Top 5 Processes\n");
        printf("4. Continuous Monitoring\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }

        if (choice == 1) getCPUUsage();
        else if (choice == 2) getMemoryUsage();
        else if (choice == 3) listTopProcesses();
        else if (choice == 4) {
            int sec;
            printf("Interval (seconds): ");
            if (scanf("%d", &sec) !=1 || sec <= 0) {
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
        else printf("Invalid choice.\n");
    }
}

//Main function and overall module integration
int main(int argc, char *argv[]) {
    signal(SIGINT, handleSignal);

    if (argc > 1) {
        // Command-line argument handling
        if (strcmp(argv[1], "-m") == 0) {
             if (argc < 3) {
                 // Error handling logic from Member 3's task
                 printf("Error: missing parameter. Use -m [cpu/mem/proc]\n");
                 return 1;
             }
             if (strcmp(argv[2], "cpu") == 0) getCPUUsage();
             else if (strcmp(argv[2], "mem") == 0) getMemoryUsage();
             else if (strcmp(argv[2], "proc") == 0) listTopProcesses();
             else printf("Invalid argument for -m.\n");
             return 0;
        }

        if (strcmp(argv[1], "-c") == 0) {
            if (argc < 3) {
                 // Error handling logic from Member 3's task
                 printf("Error: missing parameter. Use -c <seconds>\n");
                 return 1;
            }
            int sec = atoi(argv[2]);
            if (sec <= 0) sec = 2; 
            continuousMonitor(sec);
            return 0;
        }

        // Error handling logic from Member 3's task
        printf("Invalid option. Use -h for help.\n");
        return 1; 
    }

    showMenu();
    return 0;
}

