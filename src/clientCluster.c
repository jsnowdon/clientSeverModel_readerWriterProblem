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

    int numOfReaders, numOfWriters, iterations, i, rc;
    char filename[20];

    numOfWriters = my_data->numOfWriters;
    numOfReaders = my_data->numOfReaders;
    iterations = my_data->iterations;
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

    int threadID, iterations, bufferSize, i, j;
    FILE *fp;
    int temp;
    char intString[sizeof(int)];
    char filename[20];

    /*Get data from struct*/
    threadID = my_data->thread_id;
    iterations = my_data->iterations;
    bufferSize = my_data->numOfWriters;
    strcpy(filename, my_data->filename);

    for( i = 0; i < iterations; i++)
    {

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

        printf("I am thread #%d, iteration #%d and the file contains: %s\n", threadID, i+1, buffer);

        /*cleanup*/
        fclose(fp);
        free(buffer);

        /*sleep for a bit*/
        sleep(rand()%2);
    }

    /* cleanup */
    pthread_exit((void *)threadData);
}

void *writerThreads(void *threadData)
{
    thread_data *my_data;
    my_data = (thread_data *) threadData;

    int threadID, iterations, fileSize, i, j;
    FILE *fp;
    int temp;
    char filename[20];

    /*Get data from struct*/
    threadID = my_data->thread_id;
    iterations = my_data->iterations;
    fileSize = my_data->numOfWriters;
    strcpy(filename, my_data->filename);

    for( i = 0; i < iterations; i++)
    {
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