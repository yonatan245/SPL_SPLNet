//
// Created by hodyo@wincs.cs.bgu.ac.il on 16/01/2020.
//

#include "../include/Readers.h"
#include <boost/thread/thread.hpp>
using namespace std;

void Readers::readFromSocket(ConnectionHandler *connectionHandler, bool* terminated, ConcurrentQueue* messageQueue, Client *client) {
    while (!*terminated) {

        std::string message = "";
        std::string command;

        connectionHandler->getLine(message);

        std::stringstream messageStream(message);
        messageStream >> command;

        if(command == "CONNECTED") client->connectedReceived();
        else if(command == "ERROR") client->errorReceived(&messageStream);
        else if(command == "MESSAGE") client->messageReceived(&messageStream);
        else if(command == "RECEIPT") client->receiptReceived(&messageStream, connectionHandler);
    }
}

void Readers::readFromKeyboard(ConnectionHandler *connectionHandler, bool* terminated, ConcurrentQueue* messageQueue, Client *client) {
    boost::this_thread::interruption_enabled();
    while (!*terminated) {
        boost::this_thread::interruption_point();
        std::string input, outputStomp;
        getline(cin, input);

        std::stringstream stream(input);

        std::string command;
        stream >> command;

        try {
            if (command == "login") client->login(&stream, connectionHandler);
            else if (command == "join") client->joinGenre(&stream);
            else if (command == "exit") client->exitGenre(&stream);
            else if (command == "add") client->addBook(&stream);
            else if (command == "borrow") client->borrowBook(&stream);
            else if (command == "return") client->returnBook(&stream);
            else if (command == "status") client->genreBookStatus(&stream);
            else if (command == "logout") {
                client->logoutUser(connectionHandler);
                break;
            } else cout << "Invalid input\n";

        } catch (logic_error) { terminateNow(client); }
    }
}

void Readers::sendMessage(ConnectionHandler *connectionHandler, std::string message, Client *client) {

        const short bufsize = 1024;
        char buf[bufsize];
        int len = message.length();

        if (!connectionHandler->sendLine(message)) {
            terminateNow(client);
        }
    }

void Readers::terminateNow(Client *client) {
        client->terminateNow();
    }
    
