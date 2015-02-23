/*
Queue Implmentation from: Data Structures, Algorithms & Software Principles in C
Author of the book: Thomas A. Standish
*/

#include <stdio.h>
#include <stdlib.h>
#include "../include/header.h"

void initializeQueue(Queue *Q)
{
	Q->front = NULL;
	Q->rear = NULL;
	Q->numOfReaders = 0;
	Q->writerPresent = 0;
}

int empty(Queue *Q)
{
	return ( Q->front == NULL );
}

void insertItem(itemType R, Queue *Q)
{
	QueueNode *Temp;

	/* allocate room for new node */
	Temp = (QueueNode *)malloc(sizeof(QueueNode));

	/* check for failures */
	if ( Temp == NULL )
	{
		printf("Error allocating memory for queue\n");
		exit(0);
	}

	Temp->item = R;
	Temp->link = NULL;

	/* check if first element in queue */
	if( Q->rear == NULL )
	{
		Q->front = Temp;
		Q->rear = Temp;
	}
	else
	{
		Q->rear->link = Temp;
		Q->rear = Temp;
	}

}

void removeItem(Queue *Q, itemType *F)
{
	QueueNode *Temp;

	/* Make sure the queue is not empty */
	if ( Q->front == NULL )
	{
		printf("Attempting to remove item from empty list\n");
	}
	else
	{
		*F = Q->front->item;
		Temp = Q->front;
		Q->front = Temp->link;
		free(Temp);

		/* if this is the last element removed reset queue to NULL */
		if ( Q->front == NULL ){
			Q->rear = NULL;
		}
	}
}

void peakAtHead(Queue *Q, itemType *head)
{
	itemType temp;

	/* if queue is empty return random item with empty flag set */
	if( Q->front == NULL )
	{
		/* set flag to notify caller of empty queue */
		temp.isEmpty = 1;
		*head = temp;
	}
	else
	{
		*head = Q->front->item;
	}
}

void getNextElement(QueueNode *Q, itemType *item)
{
	/* get the item at that position in the queue */
	*item = Q->item;
}

void setWriterFlag(Queue *Q)
{
	Q->writerPresent = 1;
}

void unsetWriterFlag(Queue *Q)
{
	Q->writerPresent = 0;
}

void incrementReaders(Queue *Q)
{
	Q->numOfReaders++;
}

void decrementReaders(Queue *Q)
{
	Q->numOfReaders--;
}