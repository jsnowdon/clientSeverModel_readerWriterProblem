/* 
 * CIS*4410 Assignment 2
 *
 * Author: Jonathan Snowdon
 *
 * Student#: 0709496
 *
 */

#ifndef _HEADER_H_
#define _HEADER_H_

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

 #define PORT_NUM 3333
 #define SERVER_ACK "ack"
 #define MAX_NUMOFQUEUES 100

 #pragma pack(1)   // this helps to pack the struct to 5-bytes
typedef struct packet{

	int clusterID;
	int isWriting;
	int isRequest;
	char filename[20];

}msg;
#pragma pack(0)   // turn packing off

 typedef struct item{

	struct sockaddr_in client_addr; /* client address */
    socklen_t addrlen;
    msg clientMessage;
    int isEmpty;
    int beenCalled;

}itemType;

typedef struct QueueNodeTag{
	itemType item;
	struct QueueNodeTag *link;
}QueueNode;

typedef struct{
	QueueNode *front;
	QueueNode *rear;
	int numOfReaders;
	int writerPresent;
}Queue;

 Queue queueArray[MAX_NUMOFQUEUES];

void initializeQueue(Queue *Q);
int empty(Queue *Q);
void insertItem(itemType R, Queue *Q);
void removeItem(Queue *Q, itemType *F);
void peakAtHead(Queue *Q, itemType *head);
void getNextElement(QueueNode *Q, itemType *item);
void setWriterFlag(Queue *Q);
void unsetWriterFlag(Queue *Q);
void incrementReaders(Queue *Q);
void decrementReaders(Queue *Q);

int alreadyHasQueue(int clusterID, int numOfQueues);
void sendServerAck(int socketID, itemType clientInfo, Queue *Q);
void addItemToNewQueue(itemType clientInfo, int *numOfQueues);
void checkQueue(int socketID, itemType clientInfo, int index );

#endif