CC      = gcc
CFLAGS  = -Wall -Werror -std=c99 -pthread
RM      = rm -f

default:all

all:a3q2 shutdownServer

a3q2:
	$(CC) $(CFLAGS) -o a3q2 a3q2.c

shutdown:
	$(CC) $(CFLAGS) -o shutdownServer shutdownServer.c

clean:
	$(RM) a3q2 shutdown
