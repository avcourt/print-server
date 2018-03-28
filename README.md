## PrintServer

This is essentially an implementation of the producer/consumer problem modelled as printServers/printClients.
It uses `SysV semaphores` to implement the buffer which is stored in a shared memory segment.

Each client and server procress is created using `fork()`.
Each printClient makes 6 print requests.
- Filename is the client's PID with the loop iteration appended to it,
    - e.g. `Printclient:12345`'s 3rd print request file would be `123453`.
- When serving a request, the printServer sleeps for a random number of seconds between 1-4 to simulate printing files of various sizes.


 You will be prompted to input the number of servers and clients which will then fork the inputted number of client/server processes off the parent in the main function.
 Provide the flag -n when running the program to be prompted for num of servers and clients.(Actually, providing any command line argument will cause the program to prompt you for the numServers/Clients)


If no command line args are provided, the program runs with the default single server and client.

The printServer processes catch signals and can be killed by running `shutdownServer.c` with the server processID as an argument. 
<hr>

#### Usage
Compile both programs using make:
```
make
./printQueue -n
```
To shutdown a printServer from the running printQueue program:

`./shutdown <serverPID>`

*where serverPID will be an id shown in the output of `printQueue`*

This is best demonstrated while using multiple printServers with an active print queue.
If all print jobs have been placed on the queue (all clients are 'offline'), killing
each server through `shutdown.c`, `printQueue` terminates cleanly.
