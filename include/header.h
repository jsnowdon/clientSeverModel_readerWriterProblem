/* 
 * CIS*4410 Assignment 2
 *
 * Author: Jonathan Snowdon
 *
 * Student#: 0709496
 *
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h> 

#define FILENAME "./file.bin"
#define PORT_NUM 3333;

#pragma pack(1)   // this helps to pack the struct to 5-bytes
typedef struct packet{

	int id;
	int isWriting;
	char filename[20];

}msg;
#pragma pack(0)   // turn packing off

typedef struct threadInfo{

   int  thread_id;
   int iterations;
   int numOfWriters;
   int numOfReaders;
   char filename[20];

} thread_data;

/* Get input from user */
void getUserInput(int *numOfClusters, int *numOfReaders, int *numOfWriters, int *iterations);

/* Create the file */
void initFile(int numOfWriters, char * filename);

/* pthread function for clusters */
void *clusterThreads(void *threadData);

/* pthread function for readers */
void *readerThreads(void *threadData);

/* pthread function for writers */
void *writerThreads(void *threadData);