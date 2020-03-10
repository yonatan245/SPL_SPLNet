# SPL_SPLNet
Assignment 3 in System Prgramming course

In this assignment, we've created a server (eritten in Java) and a Client side (written in C++) which mimics a books club, using STOMP messages based communication.

This project was mainly about understanding the basic ideas and construction of the server, so it is relies on modifications of existing code which was taught in class.

The server can be used in 2 forms: as TPC, or as a Reactor.
for activating the server, the file StompServer.java sould be run with the arguments:  [IP and port number] [server type] , when server type is "tpc" for thread per client, or "reactor" for Reactor.
  
Each client can log in, but once the client logs out it's information is lost.
Each client can add books to the club, sign to preferred genres, lend books and have a books status check - which makes all the clients who are registered to a genre mention which books the own in the moment.
Lending books is a transitive operation, for example: If Bob has added the book 1984 and Alice has landed it, Carl can borrow the book from Alice, and then he returns it to Alice once he's done with it.

All of the legal actions are mentioned with examples in the assignment specifications, page 7.
