#include <thread>
#include "../include/Readers.h"
#include <iostream>
#include "../include/ConnectionHandler.h"
#include "../include/Client.h"
#include "../include/ConcurrentQueue.h"
#include <boost/thread/thread.hpp>

using namespace std;

/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
int main (int argc, char *argv[]) {

    //Initiate client and input threads
    Client *client = new Client();
    bool *terminated = client->getIsTerminated();
    ConcurrentQueue *messageQueue = client->getMessageQueue();
    ConnectionHandler *connectionHandler;

    std::string command;
    do {
        std::string input;
        getline(cin, input);

        std::stringstream stream(input);
        stream >> command;

        if(command == "login"){
            std::string hostPort, username, password;

            stream >> hostPort;
            stream >> username;
            stream >> password;

            int devider = hostPort.find(':');
            std::string host = hostPort.substr(0, devider);
            short port = stoi(hostPort.substr(devider +1, hostPort.length() - devider));

            client->setUserName(username);

            connectionHandler = new ConnectionHandler(host, port);

            if (!connectionHandler->connect()) {
                std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
                return 0;
            }
            else cout << "Connected!" << endl;

            std::string output = "CONNECT\n"
                   "accept-version: 1.2\n"
                   "host: stomp.cs.bgu.ac.il\n"
                   "login: " +username +"\n" +
                   "passcode: " +password +"\n" +
                   "\0";

            messageQueue->push(output);
        }
    } while(command != "login");

    boost::thread keyboardReader(Readers::readFromKeyboard, connectionHandler, terminated, messageQueue, client);
    boost::thread socketReader(Readers::readFromSocket, connectionHandler, terminated, messageQueue, client);

    //Send message from the message Queue, wait for result
    while (!*terminated) {

        std::string message = messageQueue->ConcurrentQueue::pop();

        if(message == "ERROR") {
            client->logoutUser(connectionHandler);
            break;
        }

        Readers::sendMessage(connectionHandler, message, client);
    }
    keyboardReader.interrupt();
    socketReader.join();

    delete client;
    delete connectionHandler;

    return 0;
}





