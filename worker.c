#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

const int BUFF_SZ = sizeof(int)*2;
int shm_key;
int shm_id;

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        fprintf(stderr, "Usage: worker sec nonosec\n");
        exit(1);
    }
    
    int run_sec = atoi(argv[1]);
    int run_nano = atoi(argv[2]);
    
    printf("WORKER PID:%d PPID:%d\n", getpid(), getppid());
    printf("Interval: %d seconds, %d nanoseconds\n", run_sec, run_nano);
    
    // Shared memory
    int shm_key = ftok("oss.c", 'R');
    if (shm_key <= 0 ) {
        fprintf(stderr,"Child:... Error in ftok\n");
        exit(1);
    }
    
    // Create shared memory segment
    int shm_id = shmget(shm_key,BUFF_SZ,0700);
    if (shm_id <= 0 ) {
        fprintf(stderr,"child:... Error in shmget\n");
        exit(1);
    }
    
    // Attach to the shared memory segment
    int *clock = (int *)shmat(shm_id,0,0);
    if (clock == (void *) -1) {
        fprintf(stderr,"Child:... Error in shmat\n");
        exit(1);
    }
    
    // Access the shared memory
    int *sec = &(clock[0]);
    int *nano = &(clock[1]);

    int startSec = *sec;
    int startNano = *nano;

    int termSec = startSec + runSec;
    int termNano = startNano + runNano;

    if (termNano >= 1000000000) {
        termSec++;
        termNano -= 1000000000;
    }

    int lastPrintedSec = startSec;

    while (1) {

        if (*sec > termSec || (*sec == termSec && *nano >= termNano)) {
            printf("WORKER PID:%d PPID:%d\n", getpid(), getppid());
            printf("SysClockS:%d SysClockNano:%d TermTimeS:%d TermTimeNano:%d\n", startSec, startNano, termSec, termNano);
            printf("--Just Starting\n");
            break;
        }

        if (*sec > lastPrintedSec) {
            printf("WORKER PID:%d PPID:%d\n", getpid(), getppid());
            printf("SysClockS:%d SysClockNano:%d TermTimeS:%d TermTimeNano:%d\n", *sec, *nano, termSec, termNano);
            printf("--%d seconds have passed since starting\n", *sec - startSec);
            lastPrintedSec = *sec;
        }
    }

    shmdt(clock);
    return 0;
}
