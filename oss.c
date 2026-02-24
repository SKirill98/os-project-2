#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int opt;
    int n = -1, s = -1, t = -1, i = -1; // Initialize to -1

    // Shared memory
    const int BUFF_SIZE = sizeof(int) * 2;
    int shm_key;
    int shm_id;

    // PCB
    struct PCB {
        int occupied; // 0 for free, 1 for occupied
        pid_t pid; // Process ID of this child
        int start_sec; // Start time in seconds
        int start_nanosec; // Start time in nanoseconds
        int ending_time_sec; // Ending time in seconds
        int ending_time_nanosec; // Ending time in nanoseconds
    }

    while ((opt = getopt (argc, argv, "hn:s:t:i:")) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: %s [-h] [-n proc] [-s simul] [-t timelimitForChildren] [-i intervalInSecondsToLaunchChildren]\n", argv[0]);
                printf("  -h: Display this help message and exit\n");
                printf("  -n proc   Total number of child processes to launch\n");
				printf("  -s simul  Maximum number of children running simultaneously\n");
				printf("  -t iter   The amount of simulated time each child runs\n");
                printf("  -i sec    Interval in simulated seconds to launch new children\n");
                return 0;
            case 'n':
                n = atoi(optarg);
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                t = atoi(optarg);
                break;
            case 'i':
                i = atoi(optarg);
                break;
            default:
                printf("Usage: %s [-h] [-n proc] [-s simul] [-t timelimitForChildren] [-i intervalInSecondsToLaunchChildren]\n", argv[0]);
                printf("  -h: Display this help message and exit\n");
                printf("  -n proc   Total number of child processes to launch\n");
				printf("  -s simul  Maximum number of children running simultaneously\n");
				printf("  -t iter   The amount of simulated time each child runs\n");
                printf("  -i sec    Interval in simulated seconds to launch new children\n");
                return 1;
        }
    }

    // Check if all required options are provided
    if (n == -1 || s == -1 || t == -1 || i == -1) {
        fprintf(stderr, "Error: Missing required options\n");
        printf("Usage: %s [-h] [-n proc] [-s simul] [-t timelimitForChildren] [-i intervalInSecondsToLaunchChildren]\n", argv[0]);
    }

    int shm_key = ftok("oss.c", 'R'); // Generate a unique key for shared memory
    if (shm_key <= 0) {
        fprintf(stderr, "Parent: Failed to generate shared memory key (ftok failed)\n");
        return 1;
    }

    int shm_id = shmget(shm_key, BUFF_SIZE, 0700|IPC_CREAT); // Create shared memory segment
    if (shm_id <= 0) {
        fprintf(stderr, "Parent: Failed to create shared memory segment (shmget failed)\n");
        return 1;
    }

    int *clock = (int *)shmat(shm_id, 0, 0); // Attach to shared memory
    if (clock <= 0) {
        fprintf(stderr, "Parent: Failed to attach to shared memory (shmat failed)\n");
        return 1;
    }

    *sec = 0; // Initialize seconds
    *nanosec = 0; // Initialize nanoseconds

    int running = 0;
    int total_launched = 0;
    int status;

    // Launch first children batch obeying process limit and time bound limits
    while (running < s && total_launched < n) {
        pid_t pid = fork();
        if (pid == 0) {
            // In child process
            execl("./worker", "worker", NULL); // Replace with actual worker executable
            fprintf(stderr, "Child: Failed to execute worker\n");
            exit(1);
        } else if (pid > 0) {
            // In parent process
            running++;
            total_launched++;
        } else {
            fprintf(stderr, "Parent: Failed to fork\n");
            return 1;
        }
    }

    /*
    while (stillShildrenToLaunch) || haveChildrenInSystem) {
        incrementClock();

        Every half a second of simulated clock time, output the process table to the screen.
        
        checkIfChildHasTermnated():

        if (childTerminated) {
            updatePCBForTerminatedChild();
            running--;
        }
        
        possiblyLaunchNewChild(); // Obeying process limit and time bound limits
    }
    */
   while (total_launched < n || running > 0) {
        // Increment clock
        (*nanosec) += 10000000; // Increment by 0.01 seconds
        if (*nanosec >= 1000000000) {
            *sec += 1;
            *nanosec -= 1000000000;
        }

        // Check for terminated children
        pid_t pid = waitpid(-1, &status, WNOHANG);
        if (pid > 0) {
            // Update PCB for terminated child (not implemented here)
            running--;
        }

        // Possibly launch new child (not implemented here)
    }

    return 0;
}
