#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>




// GLOBALS
int* buf_ptr;
int* vars; // queue control vars
int HEAD = 0;
int TAIL = 1;
short BUFSIZE = 4;
int	shmid;		// segment for queue control vars
int	buf_id;		// segment for buffer

// index for mySems array in main function
// 0=>free spaces in the buffer, 1=>num elmts curr in buffer, 2=>binary semaphore acting as a mutex
int	sem_id;


// PROTOTYPES
void printClient(int);
void printServer(int);
void sig_handler(int);



// MAIN function
int main(int argc, char* argv[]) {

	int numServers, numClients;

	// semaphore set
	struct	sembuf	mySems[] = {	// initialize the semaphores ...
		{ 0, BUFSIZE, IPC_NOWAIT },	// init spaces in buffer to BUFSIZE
		{ 1, 0, IPC_NOWAIT },		// init # of elmts in buffer to 0
		{ 2, 1, IPC_NOWAIT }		// mutex
	};


    // queue control vars
    shmid = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    vars = (int *)shmat(shmid, 0, 0);
    vars[HEAD] = 0; 
    vars[TAIL] = 0;
    
    // init buffer
    buf_id = shmget(IPC_PRIVATE, BUFSIZE * sizeof(int), IPC_CREAT | 0666);

    // init the 3 sems
    sem_id = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);
    semop(sem_id, mySems, 3);


	if(argc <= 1) {
		printf("Running with default of 1 server and 1 client\n");
		numServers = 1;
		numClients = 1;
	}
	else {
		printf("Enter # of print servers:");
		scanf("%d",&numServers);

		printf("Enter # of print clients:");
		scanf("%d",&numClients);
		printf("Running with %d server(s) and %d client(s)\n", numServers, numClients);
	}


	// create servers and clients
    for (int i = 0; i < numServers + numClients; i++) {
        int	pid = fork();

        if (pid == 0 && i < numServers)
            printServer(getpid());
        else if (pid == 0)
            printClient(getpid());
    }

	for (int i = 0; i < numServers + numClients; i++)
		wait(NULL);

	return 0;
}




void printClient(int id) {

    // 0=>free spaces in the buffer
    // 1=>num elmts curr in buffer
    // 2=>binary semaphore acting as a mutex

    // when acquiring access to CS, printclient decrememnts free spaces in buffer
	struct	sembuf	acq[] = {{ 0, -1, 0 }, { 2, -1, SEM_UNDO }};
    // when releasing access to CS, printclient increments totalNum elts in buffer
	struct	sembuf	rel[] = {{ 1, 1, 0 }, { 2, 1, SEM_UNDO }};

	vars = (int *)shmat(shmid, 0, 0);
	buf_ptr = (int *)shmat(buf_id, 0, 0);

	printf("PrintClient:%d ONLINE\n", id);
	for (int i = 0; i < 6; i++)
	{
		int	fileName = getpid() * 10 + i;

		// CS
		semop(sem_id, acq, 2); //acq mutex (sembuf[2])

		buf_ptr[vars[HEAD]] = fileName;
		vars[HEAD] = (vars[HEAD] + 1) % BUFSIZE;

		semop(sem_id, rel, 2); //rel mutex (sembuf[2])
		// End CS

        printf("printClient:%d added file:%d to buffer\n", id, fileName);
	}
    sleep(3);
	printf("printClient %d OFFLINE\n", id);
	exit(0);
}



void printServer(int id) {

	signal(SIGUSR1, sig_handler);

    // does the opposite of printClient when entering/exiting its ciritcal section
	struct	sembuf	acq[] = {{ 1, -1, 0 }, { 2, -1, SEM_UNDO }};
	struct	sembuf	rel[] = {{ 0, 1, 0 }, { 2, 1, SEM_UNDO }};

	vars = (int *)shmat(shmid, 0, 0);
	buf_ptr = (int *)shmat(buf_id, 0, 0);

	printf("PrintServer:%d ONLINE\n", id);
	for (;;) {
		int	fileName;

        // CS
		semop(sem_id, acq, 2);	// acq mutex (sembuf[2])

		fileName = buf_ptr[vars[TAIL]];
        printf("printServer:%d starting file %d\n", id, fileName);

		vars[TAIL] = (vars[TAIL] + 1) % BUFSIZE;
		semop(sem_id, rel, 2);	// release mutex (sembuf[2])
		// end CS

        sleep(rand() % 4);
		printf("printServer:%d printed file %d\n", id, fileName);
	}
}

void sig_handler(int signum) {
    if (signum == SIGUSR1) {
        printf("shutting down server:%d ...\n", getpid());
        shmdt(vars);
        exit(0);
    }
}



