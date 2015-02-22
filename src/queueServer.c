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

    int socketID;
    struct sockaddr_in server_addr; /* server address */
    struct sockaddr_in client_addr; /* client address */
    socklen_t addrlen = sizeof(client_addr);

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

    /* Recieve a message from the client */
    if ( recvfrom( socketID, (void *) &clientRequest, sizeof(clientRequest), 0, (struct sockaddr *) &client_addr, &addrlen ) < 0 )
    {
        printf("Error recieving message from client\n");
        return 0;
    }
    
    printf("Message: %s\n", clientRequest.filename);

   /* strcpy(message, "ack");

    if ( sendto( socketID, message, sizeof(message), 0, (struct sockaddr *) &client_addr, addrlen ) < 0 )
    {
        printf("Error sending message to client\n");
        return 0;
    }*/

    close( socketID );

    return 0;   
}