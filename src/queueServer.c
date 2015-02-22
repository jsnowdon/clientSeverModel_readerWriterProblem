/* 
 * CIS*4410 Assignment 2
 *
 * Author: Jonathan Snowdon
 *
 * Student#: 0709496
 *
 */

#include "../include/header.h"

 /* TODO

    create queues for each cluster
    check if the first memeber of the queue is reader or writer
    if reader let them read, and increment reader count
    once reader is done decriment reader count
    if writer, wait till reader count is 0, turn on writer flag
    once writer is done turn off writer flag
*/

int main(int argc, char *argv[])
{

    int socketID;
    struct sockaddr_in server_addr; /* server address */
    struct sockaddr_in client_addr; /* client address */
    socklen_t addrlen = sizeof(client_addr);
    char clientRelease[10];

    msg clientRequest; 

    /* Create the socket, but also ensure it worked */
    if ( ( socketID = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 ){
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

    while(1){

    /* Recieve a message from the client */
    if ( recvfrom( socketID, (void *) &clientRequest, sizeof(clientRequest), 0, (struct sockaddr *) &client_addr, &addrlen ) < 0 )
    {
        printf("Error recieving message from client\n");
        return 0;
    }
    
    printf("Message: %d, %d, %s\n", clientRequest.clusterID, clientRequest.isWriting, clientRequest.filename);

    if ( sendto( socketID, SERVER_ACK, sizeof(SERVER_ACK), 0, (struct sockaddr *) &client_addr, addrlen ) < 0 )
    {
        printf("Error sending message to client\n");
        return 0;
    }

    if ( recvfrom( socketID, clientRelease, sizeof(clientRelease), 0, (struct sockaddr *) &client_addr, &addrlen ) < 0 )
    {
        printf("Error recieving message from client\n");
        return 0;
    }

    printf("Recieved back: %s\n", clientRelease);

    }

    close( socketID );

    return 0;   
}