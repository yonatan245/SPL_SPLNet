#ifndef CLIENT_READERS_H
#define CLIENT_READERS_H


#include "Client.h"

class Readers {

    static void terminateNow(Client *client);

public:
    static void readFromKeyboard(ConnectionHandler *connectionHandler, bool* terminated, ConcurrentQueue* messageQueue, Client *client);

    static void readFromSocket(ConnectionHandler *connectionHandler, bool* terminated, ConcurrentQueue* messageQueue, Client *client);

    static void sendMessage(ConnectionHandler *connectionHandler, std::string message, Client *client);
};


#endif //CLIENT_READERS_H
