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
OBJS       = client.o
OBJS_DEBUG = client_DEBUG.o

#Execution
client: $(OBJS)
		@ $(CC) $(LFLAGS) $(OBJS) -o $(BDIR)/client

client.o: $(SDIR)/clientCluster.c $(IDIR)/header.h
		@ $(CC) $(CFLAGS) $(SDIR)/clientCluster.c -o client.o

#DEBUG Execution
client_DEBUG: $(OBJS_DEBUG)
		$(CC) $(LFLAGS) $(OBJS_DEBUG) -o $(BDIR)/client

client_DEBUG.o: $(SDIR)/clientCluster.c $(IDIR)/header.h
		$(CC) $(CFLAGS) $(SDIR)/clientCluster.c -o client_DEBUG.o

#Make Targets
all: client

run: client
		@ $(BDIR)/client

debug: client_DEBUG
		$(BDIR)/client

clean:
		@ rm $(BDIR)/client *.o
		@ rm ./file*.bin
