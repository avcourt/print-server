CC      = gcc
CFLAGS  = -Wall -Werror -std=c99 -pthread
RM      = rm -f

default:all

all:printQueue shutdownServer

printQueue:
	$(CC) $(CFLAGS) -o printQueue printQueue.c

shutdown:
	$(CC) $(CFLAGS) -o shutdownServer shutdownServer.c

clean:
	$(RM) printQueue shutdownServer
