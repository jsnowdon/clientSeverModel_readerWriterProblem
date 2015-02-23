/* 
 * CIS*4410 Assignment 2
 *
 * Author: Jonathan Snowdon
 *
 * Student#: 0709496
 *
 */

#include "../include/header.h"

int main(int argc, char *argv[])
{

    int socketID, index, numOfQueues, i;
    struct sockaddr_in server_addr; /* server address */
    struct sockaddr_in client_addr; /* client address */
    socklen_t addrlen = sizeof(client_addr);
    itemType clientInfo, queueHeadItem;
    msg clientMsg;

    /* No queues yet */
    numOfQueues = 0; 

    /* Initilize all the queues */
    for ( i = 0; i < MAX_NUMOFQUEUES; i++ )
    {
        initializeQueue(&queueArray[i]);
    }

    /* Create the socket, but also ensure it worked */
    if ( ( socketID = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 )
    {
        printf("Failure creating socket\n");
        return 0;
    }

    /* Create the servers socket name */
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT_NUM);

    /* Bind the socketID to the socket name */
    if ( bind( socketID, ( struct sockaddr * ) &server_addr, sizeof(server_addr) ) < 0 )
    {
        printf("Server bind failed\n");
        return 0;
    }

    /* Server Loop */
    while( 1 )
    {
        /* Recieve a message from the client */
        if ( recvfrom( socketID, (void *) &clientMsg, sizeof(clientMsg), 0, (struct sockaddr *) &client_addr, &addrlen ) < 0 )
        {
            printf("Error recieving message from client\n");
            return 0;
        }

        /* if server recieved a request from the client */
        if ( clientMsg.isRequest == 1 )
        {
            printf("Request: %d, %d, %s\n", clientMsg.clusterID, clientMsg.isWriting, clientMsg.filename);

            /* Create queue item */
            clientInfo.client_addr = client_addr;
            clientInfo.addrlen = addrlen;
            clientInfo.clientMessage.clusterID = clientMsg.clusterID;
            clientInfo.clientMessage.isWriting = clientMsg.isWriting;
            clientInfo.clientMessage.isRequest = clientMsg.isRequest;
            strcpy(clientInfo.clientMessage.filename, clientMsg.filename);

           // printf("id:%d, writing:%d, request:%d\n",clientMsg.clusterID, clientMsg.isWriting, clientMsg.isRequest);

            /* check if there are some queues already created */
            if ( numOfQueues > 0 ){

                /* check if there is alread a queue with the same clusterID */
                index = alreadyHasQueue(clientMsg.clusterID, numOfQueues);

                /* cluster does not yet have a queue */
                if ( index == -1 )
                {
                    /* add the item to a new queue */
                    addItemToNewQueue(clientInfo, &numOfQueues);
                    /* send server ack to client */
                    sendServerAck( socketID, clientInfo, &queueArray[numOfQueues-1]);

                }
                /* cluster already has a queue */
                else
                {
                    /* insert item into queue */
                    insertItem(clientInfo, &queueArray[index]);
                }

            }
            /* no queues yet so create 1 and then immediately send ack back */
            else
            {
                /* add the item to a new queue */
                addItemToNewQueue(clientInfo, &numOfQueues);
                /* send server ack to client */
                sendServerAck( socketID, clientInfo, &queueArray[numOfQueues-1]);
            }


        }
        /* else server recieved a reply from the client */
        else
        {
            /* since its request server must have queue with its clusterID */
            /* so get queue */
            printf("Reply: %d, %d, %s\n", clientMsg.clusterID, clientMsg.isWriting, clientMsg.filename);

            index = alreadyHasQueue(clientMsg.clusterID, numOfQueues);

            /* simple error check */
            if( index == -1)
            {
                printf("Something has gone wrong, there should be a queue with this cluster already\n");
                exit(0);
            }
            
            /* remove the item from the list */
            removeItem(&queueArray[index], &queueHeadItem);

            /* if client item removed was a writer turn off writer flag */
            if ( queueHeadItem.clientMessage.isWriting == 1 )
            {
                unsetWriterFlag(&queueArray[index]);
            }
            /* else must be a reader, derement the number of readers */
            else
            {
                decrementReaders(&queueArray[index]);
            }

            /* now look at the next members of the queue */
            checkQueue( socketID, queueHeadItem, index);
        }
    }

    close( socketID );

    return 0;   
}

int alreadyHasQueue(int clusterID, int numOfQueues)
{
    int i;
    itemType headItem;

    for( i = 0; i < numOfQueues; i++ )
    {
        peakAtHead( &queueArray[i], &headItem);

        if (  headItem.isEmpty != 1 )
        {
            /* if there is a queue already with this cluster return the queue (index in queueArray) */
            if( headItem.clientMessage.clusterID == clusterID ){
                return i;
            }
        }
        /* found queue that is empty */
        else
        {
            break;
        }
    }

    return -1;
}

void sendServerAck(int socketID, itemType clientInfo, Queue *Q)
{
    /* if the client is a writer */
    if( clientInfo.clientMessage.isWriting == 1 )
    {
        /* set writer flag */
        setWriterFlag(Q);
    }
    /* the client must be a reader */
    else
    {
        /* increment the number of readers */
        incrementReaders(Q);
    }

    /* send back ACK to client */
    if ( sendto( socketID, SERVER_ACK, sizeof(SERVER_ACK), 0, (struct sockaddr *) &clientInfo.client_addr, clientInfo.addrlen ) < 0 )
    {
        printf("Error sending message to client\n");
        exit(0);
    }

}

void addItemToNewQueue(itemType clientInfo, int *numOfQueues)
{
    /* add item into first avaible queue */
    insertItem(clientInfo, &queueArray[*numOfQueues]);
    /* increase the number of queues */
    (*numOfQueues)++;
}

void checkQueue(int socketID, itemType clientInfo, int index)
{
    itemType item;
    Queue *Q = &queueArray[index];

    QueueNode *Temp;

    /* allocate room for new node */
    Temp = (QueueNode *)malloc(sizeof(QueueNode));

    /* check for failures */
    if ( Temp == NULL )
    {
        printf("Error allocating memory for queue\n");
        exit(0);
    }

    /* get the head element */
    Temp = Q->front;

    /* while the list has elements */
    while ( Temp != NULL )
    {

        /* get the element */
        getNextElement(Temp, &item);

        /* found a writer */
        if ( item.clientMessage.isWriting == 1 )
        {
            /* if there are no readers in the file */
            if ( Q->numOfReaders == 0 )
            {
                /* if there are no writers in the file either */
                if ( Q->writerPresent == 0 )
                {
                    /* let writer have access */
                    sendServerAck(socketID, item, &queueArray[index]);
                }
            }
        }
        /* found a reader */
        else
        {
            if ( item.beenCalled == 1 )
            {
                /* no need to send another ack msg */
            }
            else
            {
                /* Set flag for been called */
                item.beenCalled = 1;
                /* let the reader have access */
                sendServerAck(socketID, item, &queueArray[index]);
            }
        }

        /* go to next element in the queue */
        Temp = Temp->link;
    }
}