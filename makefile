# MAKEFILE
# By: Jonathan Snowdon 0709496

#Compile Flags
CC     = gcc
CFLAGS = -Wall -g -c -pedantic -std=c99
LFLAGS = -Wall -g -pthread -std=c99
LIBS   = -lm

#Directory Flags
SDIR = ./src
IDIR = ./include
BDIR = ./bin

#Objects
CLIENT_OBJS       = client.o
CLIENT_OBJS_DEBUG = client_DEBUG.o
SERVER_OBJS = server.o queue.o
SERVER_OBJS_DEBUG = server_DEBUG.o queue_DEBUG.o

#Make Targets
all: queue client server

debug: queue_DEBUG client_DEBUG server_DEBUG

clean:
		@ rm ./file*.bin
		@ rm $(BDIR)/client *.o
		@ rm $(BDIR)/server *.o
		@ rm $(BDIR)/queue *.o

#Execution
client: $(CLIENT_OBJS)
		@ $(CC) $(LFLAGS) $(CLIENT_OBJS) -o $(BDIR)/client

client.o: $(SDIR)/clientCluster.c $(IDIR)/clientCluster.h
		@ $(CC) $(CFLAGS) $(SDIR)/clientCluster.c -o client.o

#DEBUG Execution
client_DEBUG: $(CLIENT_OBJS_DEBUG)
		$(CC) $(LFLAGS) $(CLIENT_OBJS_DEBUG) -o $(BDIR)/client

client_DEBUG.o: $(SDIR)/clientCluster.c $(IDIR)/clientCluster.h
		$(CC) $(CFLAGS) $(SDIR)/clientCluster.c -o client_DEBUG.o

#Execution
server: $(SERVER_OBJS)
		@ $(CC) $(LFLAGS) $(SERVER_OBJS) -o $(BDIR)/server

server.o: $(SDIR)/queueServer.c $(IDIR)/header.h
		@ $(CC) $(CFLAGS) $(SDIR)/queueServer.c -o server.o

#DEBUG Execution
server_DEBUG: $(SERVER_OBJS_DEBUG)
		$(CC) $(LFLAGS) $(SERVER_OBJS_DEBUG) -o $(BDIR)/server

server_DEBUG.o: $(SDIR)/queueServer.c $(IDIR)/header.h
		$(CC) $(CFLAGS) $(SDIR)/queueServer.c -o server_DEBUG.o

#Execution
queue: $(SERVER_OBJS)
		@ $(CC) $(LFLAGS) $(SERVER_OBJS) -o $(BDIR)/queue

queue.o: $(SDIR)/queue.c $(IDIR)/header.h
		@ $(CC) $(CFLAGS) $(SDIR)/queue.c -o queue.o

#DEBUG Execution
queue_DEBUG: $(SERVER_OBJS_DEBUG)
		$(CC) $(LFLAGS) $(SERVER_OBJS_DEBUG) -o $(BDIR)/queue

queue_DEBUG.o: $(SDIR)/queue.c $(IDIR)/header.h
		$(CC) $(CFLAGS) $(SDIR)/queue.c -o queue_DEBUG.o
