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

