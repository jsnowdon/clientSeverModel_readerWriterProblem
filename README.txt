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
