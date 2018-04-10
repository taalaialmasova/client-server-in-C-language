CC = cc
COPS =
LINT = lint
LOPS = -x -u
RTLIB = -lrt

all: clients server

realtime: clockrealtimetiming clockrealtimetest abstime sendsigqueue sigqueuehandler tmrtimer timesignals periodicmessage

clients: clients.c restart.c restart.h 
	$(CC) $(COPS) -o clients clients.c restart.c -pthread -lm

server: server.c restart.c restart.h 
	$(CC) $(COPS) -o server server.c restart.c matrix.c -lm -pthread


clean:
	rm -f core *.o clients server 