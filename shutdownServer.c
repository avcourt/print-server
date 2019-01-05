#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    	if(argc<=1) {
        	printf("Usage: ./shutdown <pid_to_kill>\n");
        	exit(1);
     	}
     	int pid = atoi(argv[1]);
	printf("You provided %d as pID\n", pid);
        printf("Sending kill signal to process:%d\n", pid);


	kill(pid, SIGUSR1);

	return 0;
	
                                
 }
