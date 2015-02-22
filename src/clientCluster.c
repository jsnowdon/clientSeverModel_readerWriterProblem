/* 
 * CIS*4410 Assignment 2
 *
 * Author: Jonathan Snowdon
 *
 * Student#: 0709496
 *
 */

#include "../include/header.h"

/* TODO:
    create sockets for each reader and writer combo
    send requests to server
    wait for server reply

*/
int main(int argc, char *argv[])
{

    int numOfClusters = 0, numOfReaders = 0, numOfWriters = 0, iterations = 0, rc;
    int i;

    /*Get the number of clusters of reader and writer threads from the user
      along with the number of iterations*/
    getUserInput(&numOfClusters, &numOfReaders, &numOfWriters, &iterations);

    /*Create the clusters of reader and writer threads*/
    pthread_t clusters[numOfClusters];

    thread_data cluster_data_array[numOfClusters];

    for( i = 0; i < numOfClusters; i++ )
    {
        /*initialize struct*/
        cluster_data_array[i].thread_id = i+1;
        cluster_data_array[i].cluster_id = i+1;
        cluster_data_array[i].iterations = iterations;
        cluster_data_array[i].numOfWriters = numOfWriters;
        cluster_data_array[i].numOfReaders = numOfReaders;
        snprintf(cluster_data_array[i].filename, sizeof(char) * 32, "file%i.bin", i+1);

        /*create the pthread*/
        rc = pthread_create(&clusters[i], NULL, clusterThreads, &cluster_data_array[i]);

        /*ensure no errors*/
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    /* begin cleanup to avoid memory leaks */
    /* join all the clusters pthreads so no memory leaks */
    for( i = 0; i < numOfClusters; i++ )
    {
        pthread_join(clusters[i], NULL);
    }

    /* cleanup */
    pthread_exit(NULL);

    return 0;

} 

void *clusterThreads(void *threadData)
{
    thread_data *my_data;
    my_data = (thread_data *) threadData;

    int numOfReaders, numOfWriters, iterations, i, rc, clusterID;
    char filename[20];

    numOfWriters = my_data->numOfWriters;
    numOfReaders = my_data->numOfReaders;
    iterations = my_data->iterations;
    clusterID = my_data->cluster_id;
    strcpy(filename, my_data->filename);

    /*Initialize file*/
    initFile(numOfWriters, filename);

    /*Create the reader threads*/
    pthread_t readers[numOfReaders];

    /*Create an array of structures for the readers*/
    thread_data reader_data_array[numOfReaders];

    for( i = 0; i < numOfReaders; i++ )
    {
        /*initialize struct*/
        reader_data_array[i].thread_id = i+1;
        reader_data_array[i].iterations = iterations;
        reader_data_array[i].numOfWriters = numOfWriters;
        reader_data_array[i].numOfReaders = numOfReaders;
        reader_data_array[i].cluster_id = clusterID;
        strcpy(reader_data_array[i].filename, filename);


        /*create the pthread*/
        rc = pthread_create(&readers[i], NULL, readerThreads, &reader_data_array[i]);

        /*ensure no errors*/
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    /*Create the reader threads*/
    pthread_t writers[numOfWriters];

    thread_data writer_data_array[numOfWriters];

    for( i = 0; i < numOfWriters; i++ )
    {

        /*initialize struct*/
        writer_data_array[i].thread_id = i+1;
        writer_data_array[i].iterations = iterations;
        writer_data_array[i].numOfWriters = numOfWriters;
        writer_data_array[i].numOfReaders = numOfReaders;
        writer_data_array[i].cluster_id = clusterID;
        strcpy(writer_data_array[i].filename, filename);

        /*create the pthread*/
        rc = pthread_create(&writers[i], NULL, writerThreads, &writer_data_array[i]);

        /*ensure no errors*/
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    /* begin cleanup to avoid memory leaks */

    /* join all the reader the pthreads so no memory leaks */
    for( i = 0; i < numOfReaders; i++ )
    {
        pthread_join(readers[i], NULL);
    }

    /* join all the writer the pthreads so no memory leaks */
    for( i = 0; i < numOfWriters; i++ )
    {
        pthread_join(writers[i], NULL);
    }

    /* cleanup */
    pthread_exit(NULL);

} 

void *readerThreads(void *threadData)
{

    thread_data *my_data;
    my_data = (thread_data *) threadData;

    int threadID, clusterID, iterations, bufferSize, i, j;
    FILE *fp;
    int temp;
    char intString[sizeof(int)];
    char filename[20];
    char msgFromServer[5];

    /* Network variavbles */
    int socketID;
    struct sockaddr_in server_addr; /* server address */
    struct sockaddr_in client_addr; /* client address */
    socklen_t addrlen = sizeof(server_addr);

    /* Message structure to pass to server */
    msg msgToServer;

    /*Get data from struct*/
    threadID = my_data->thread_id;
    iterations = my_data->iterations;
    bufferSize = my_data->numOfWriters;
    clusterID = my_data->cluster_id;
    strcpy(filename, my_data->filename);

    /* Create the socket, but also ensure it worked */
    if ( ( socketID = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 ){
        printf("Failure creating socket\n");
        return 0;
    } 

    /* Create client address name */
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = 0;

    /* bind the address name with the socket  */
    if ( bind(socketID, (struct sockaddr*)&client_addr,sizeof(client_addr)) < 0 )
    {
        printf("Client bind failed\n");
        return 0;
    }

    /* get server information */
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT_NUM);

    /* create request message for server */
    msgToServer.clusterID = clusterID;
    msgToServer.isWriting = 0;
    strcpy(msgToServer.filename, filename);

    for( i = 0; i < iterations; i++)
    {
        /* send request to read the file to server */
        if ( sendto(socketID, (void *) &msgToServer, sizeof(msgToServer), 0,(struct sockaddr *) &server_addr, addrlen ) < 0 ){
            printf("Error sending message to server\n");
            return 0;
        }

        /* wait for server to send back 'ack' */
        if ( recvfrom(socketID, msgFromServer, sizeof(msgFromServer), 0,(struct sockaddr *) &server_addr, &addrlen) < 0 )
        {
            printf("Error recieving message from server\n");
            return 0;
        }

        printf("Server says: %s\n", msgFromServer); 

        /*create buffer*/
        char *buffer = (char *)calloc(bufferSize,sizeof(int));

        fp = fopen(filename, "rb+");

        /*get all integers from file*/
        for( j = 0; j < bufferSize; j++)
        {
            /*move to correct integer*/
            fseek(fp,sizeof(int)*j,SEEK_SET);
            /*read integer into temp*/
            fread(&temp, sizeof(int), 1, fp);
            /*convert integer to string*/
            sprintf(intString, "%d", temp);

            /*add new interger string to buffer*/
            strncat(buffer,intString,sizeof(int));

            /*rewind the filepointer*/
            rewind(fp);
        }

        printf("I am part of cluster #%d, thread #%d, iteration #%d and the file contains: %s\n", clusterID, threadID, i+1, buffer);

        /*cleanup*/
        fclose(fp);
        free(buffer);

        /* Tell server we are done with the file */
        if ( sendto(socketID, CLIENT_RELEASE, sizeof(CLIENT_RELEASE), 0,(struct sockaddr *) &server_addr, addrlen ) < 0 ){
            printf("Error sending message to server\n");
            return 0;
        }

        /*sleep for a bit*/
        sleep(rand()%2);
    }

    /* cleanup */
    close(socketID);
    pthread_exit((void *)threadData);
}

void *writerThreads(void *threadData)
{
    thread_data *my_data;
    my_data = (thread_data *) threadData;

    int threadID, clusterID, iterations, fileSize, i, j;
    FILE *fp;
    int temp;
    char filename[20];
    char msgFromServer[5];

    /* Network variavbles */
    int socketID;
    struct sockaddr_in server_addr; /* server address */
    struct sockaddr_in client_addr; /* client address */
    socklen_t addrlen = sizeof(server_addr);

    /* Message structure to pass to server */
    msg msgToServer;

    /*Get data from struct*/
    threadID = my_data->thread_id;
    iterations = my_data->iterations;
    fileSize = my_data->numOfWriters;
    clusterID = my_data->cluster_id;
    strcpy(filename, my_data->filename);

    /* Create the socket, but also ensure it worked */
    if ( ( socketID = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 ){
        printf("Failure creating socket\n");
        return 0;
    } 

    /* Create client address name */
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = 0;

    /* bind the address name with the socket  */
    if ( bind(socketID, (struct sockaddr*)&client_addr,sizeof(client_addr)) < 0 )
    {
        printf("Client bind failed\n");
        return 0;
    }

    /* get server information */
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT_NUM);

    /* create request message for server */
    msgToServer.clusterID = clusterID;
    msgToServer.isWriting = 1;
    strcpy(msgToServer.filename, filename);

    for( i = 0; i < iterations; i++)
    {
        /* send request to read the file to server */
        if ( sendto(socketID, (void *) &msgToServer, sizeof(msgToServer), 0,(struct sockaddr *) &server_addr, addrlen ) < 0 ){
            printf("Error sending message to server\n");
            return 0;
        }

        /* wait for server to send back 'ack' */
        if ( recvfrom(socketID, msgFromServer, sizeof(msgFromServer), 0,(struct sockaddr *) &server_addr, &addrlen) < 0 )
        {
            printf("Error recieving message from server\n");
            return 0;
        }

        printf("Server says: %s\n", msgFromServer); 

        fp = fopen(filename, "rb+");

        /*get all integers from file*/
        for( j = 0; j < fileSize; j++)
        {

            /*read integer into temp*/
            fseek(fp,sizeof(int)*j,SEEK_SET);
            fread(&temp, sizeof(int), 1, fp);
            
            /*rewind the filepointer*/
            rewind(fp);

            /*get the writers integer from the file*/
            if(threadID == j+1)
            {
                /*increment the number*/
                temp = temp + 1;
                /*write the new integer to the file*/
                fseek(fp,sizeof(int)*j,SEEK_SET);
                fwrite(&temp,sizeof(int),1, fp);

                /*rewind the filepointer*/
                rewind(fp);

                /*make sure no errors occured*/
                if(ferror(fp))
                {
                    printf("Error writing to file\n");
                    exit(-1);
                }
            }
        }

        /*cleanup*/
        fclose(fp);

        /* Tell server we are done with the file */
        if ( sendto(socketID, CLIENT_RELEASE, sizeof(CLIENT_RELEASE), 0,(struct sockaddr *) &server_addr, addrlen ) < 0 ){
            printf("Error sending message to server\n");
            return 0;
        }

        /*sleep for a bit*/
        sleep(rand()%2);
    }

    /* cleanup */
    pthread_exit((void *)threadData);
}

void getUserInput(int *numOfClusters, int *numOfReaders, int *numOfWriters, int *iterations)
{
    /*Get user input*/
    printf("Please enter the number of clusters:\n");
    scanf("%d", numOfClusters);

    printf("Please enter the number of Reader Threads per cluster:\n");
    scanf("%d", numOfReaders);

    printf("Please enter the number of Writer Threads per cluster:\n");
    scanf("%d", numOfWriters);

    printf("Please enter the number of iterations:\n");
    scanf("%d", iterations);

    printf("---------------------------------------\n");
}

void initFile(int numOfWriters, char * filename)
{
    int i, placeHolder = 0;
    /*open the file for writting*/
    FILE *fp = fopen(filename, "wb");

    /*check that the file opened correctly*/
    if(!fp)
    {
        printf("Error opening file\n");
        exit(-1);
    }

    /*print a 0 to the file for every writer*/
    for ( i = 0; i < numOfWriters; i++)
    {
        /*write the zero to the binary file*/
        fwrite(&placeHolder,sizeof(int),1, fp);
        /*make sure no errors occured*/
        if(ferror(fp))
        {
            printf("Error writing to file\n");
            exit(-1);
        }
    }

    fclose(fp);
}