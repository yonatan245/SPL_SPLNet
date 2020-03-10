#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <string>
#include <map>
#include "ConcurrentQueue.h"
#include "ConnectionHandler.h"

class Client {
private:

    //Fields
    std::string userName;
    bool terminate;
    ConcurrentQueue *messageQueue;
    //A map of books by genre. Each book is a tuple: book name, lender (me if i added the book), do we currently hold the book
    std::map<std::string, std::vector<std::tuple<std::string, std::string, bool>>> *bookInventory;
    std::map<int, std::string> *subscriptions;
    //A map of sent messages that are pending for a receipt from the server. Each message is a pair: the message, it's type
    std::map<int, std::pair<std::string, std::string>> *pendingForReceipt;
    //A vector of books we are pending to borrow
    std::vector<std::string> *pendingBooks;
    int receiptID;
    int subscriptionID;

public:
    //Constructor
    Client();

    //Destructor
    ~Client();


    //Methods
    std::string getUserName();

    ConcurrentQueue* getMessageQueue();

    std::map<std::string, std::vector<std::tuple<std::string, std::string, bool>>>* getBookInventory();

    void login(std::stringstream *stream, ConnectionHandler *ConnectionHandler);
    void joinGenre(std::stringstream *stream);
    void exitGenre(std::stringstream *stream);
    void addBook(std::stringstream *stream);
    void borrowBook(std::stringstream *stream);
    void returnBook(std::stringstream *stream);
    void genreBookStatus(std::stringstream *stream);
    void logoutUser(ConnectionHandler *ConnectionHandler);

    void connectedReceived();

    void errorReceived(std::stringstream *stringstream);

    void messageReceived(std::stringstream *pstringstream);

    void receiptReceived(std::stringstream *pstringstream, ConnectionHandler *ConnectionHandler);

    std::string getMessageBody(std::string message);

    std::string getMessageHeader(const std::string &message, const std::string &header);

    void bookStatusReceived(std::string destination);

    bool isWishToBorrow(std::string body);

    void wishToBorrowReceived(std::string body, std::string destination);

    bool isSomeoneHasBook(std::string body);

    void someoneHasBookReceived(std::string body, std::string destination);

    bool isSomeoneTakingBook(std::string body);

    void someoneTakingBookReceived(std::string body, std::string destination, int subscriptionId);

    bool isSomeoneReturningBook(std::string body);

    void someoneReturningBookReceived(std::string body, std::string destination);

    int getReceiptID();

    std::string intAsString(int someNumber);

    void registerAsReceiptPending(std::string outputStomp, int receiptID, std::string type);

    void logoutReceipt(ConnectionHandler *ConnectionHandler);

    void exitGenreReceipt(std::string message);

    void joinGenreReceipt(std::string message);

    bool* getIsTerminated();

    void terminateNow();

    void setUserName(std::string userName);

    int getSubIdByGenre(std::string genre);

    void noLongerHasBook(std::string book, std::string genre);

    std::string getBookName(std::string body, int startIndex, int endIndex);

    std::string getLastWord(std::string body);
};


#endif //CLIENT_CLIENT_H
