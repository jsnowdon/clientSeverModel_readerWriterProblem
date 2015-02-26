The Reader Writer Problem (Client Server Model)
===========================
Author: Jonathan Snowdon
Student#: 0709496

How to compile the code
=======================
I have created a makefile to help with compliation of the code.
To complie the code simply type "make" (no quotes) in the command line.

How to run the code
===================
Their are 2 executables created after the 'make' compilation. both of which are 
created in the /bin folder. To run them first run the server program by typing

./bin/server

then start up the client by typing

./bin/client

The client will ask you to input the number of clusters (i.e. Groups), number of readers
per group, the number of writers per group, and the number of iterations for each.

Notes
========
The makefile has a few other useful flags:
- make clean (will remove any binaries or files not needed anymore)
- make debug (will compile the code and show the compilation commands, as well as run the code)

Known Limitations
=================
Maximun amount of groups is currently set to 100. That is a defined variable in the header.h file.
If you require more groups please up that number.

The Problem
============

Reprogram the readers and writers problem by using the client - server model and a kind of communication facility.

Your program consists of several clients (readers and writers) and a ticket server. 
Clients are grouped to read/write different files while clients in the same group still share a single file.

The ticket server receives requests from clients and manipulates tickets for up to N different files. The request message involves the following fields: the ID of the client, the type of the request (R/W), and the name of the file which the client wants to access. A transaction to obtain a ticket for accessing a file from a client is as follows:
         -request
                  READ/WRITE
                  (the request will be blocked if the ticket server could not issue a ticket because of ME issue, or succeed if it is safe to R/W.)

         -read/write

         -release
                  READ/WRITE
                  (upon release a ticket, the ticket server might select a client from blocked queue to proceed.)

Repeat the above transaction


Algorithm
=========

I created a server that has an array of queues. As the server receives a client message it checks if it is recieving a request to read/write or a release of a read/write. 

If the server recieves a request to read/write:

1. it checks if the queue for that group is empty
2. add the element to the end of the queue
3. if the queue was empty before the item was added an ACK is sent back immedately,
   otherwise no ACK is sent.
 3.a. if the ACK was sent to a reader the number of readers affiliated with that groups file is increased
 3.b. if the ACK was sent to a writer the writers flag is set to TRUE (1) for that groups file

If the server recieves a release to read/write:

1. the server removes the head of the queue from the group affiliated with the release
 1.a. if the removed item was a reader, the server decreases the number of readers affiliated with that groups file.
 1.b. if the removed item was a writer, the server sets the writer flag to FALSE (0) for that groups file.
2. the server checks if the queue is now empty.
 2.a. if the queue is empty do nothing
 2.b. if the queue is NOT empty, the server loops through the rest of the queue.
   2.b.i. if the next memeber of the queue is a writer, break from the loop and only send 
          the ACK if there are no readers currently in that file along with the writer
          flag turned to FALSE (0)
   2.b.ii. if the next memeber of the queue is a reader, send the ACK, and continue loop


This solution solved the mutual exclusion problem without actually using locks.