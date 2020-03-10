#include "../include/Client.h"
#include "../include/ConnectionHandler.h"
#include "../include/ConcurrentQueue.h"
#include <iostream>
#include <stdexcept>
#include <boost/algorithm/string.hpp>


using namespace std;

Client::Client(): userName(""), terminate(false), messageQueue(), bookInventory(),
                    subscriptions(), pendingForReceipt(), pendingBooks(), receiptID(0), subscriptionID(0){

    messageQueue = new ConcurrentQueue();
    bookInventory = new map<std::string, vector<tuple<std::string, std::string, bool>>>();
    subscriptions = new map<int, std::string>();
    pendingBooks = new vector<std::string>();
    pendingForReceipt = new map<int, pair<std::string, std::string>>;
}
Client::~Client(){
    for(auto topic : *bookInventory){
        topic.second.clear();
    }

    for(auto iterator : *bookInventory){
        iterator.second.clear();
    }

    bookInventory->clear();
    subscriptions->clear();
    pendingBooks->clear();
    pendingForReceipt->clear();

    delete bookInventory;
    delete subscriptions;
    delete pendingBooks;
    delete pendingForReceipt;
    delete messageQueue;
}

ConcurrentQueue* Client::getMessageQueue() { return messageQueue;}
std::string Client::getUserName() {return userName;}
map<std::string, vector<tuple<std::string, std::string, bool>>> *Client::getBookInventory() {return bookInventory;}
bool* Client::getIsTerminated(){ return &terminate;}

//Aid methods
std::string Client::getMessageBody(std::string message) {

    if(message.length() < 2) return "";

    char previousChar = message.at(0);
    char currentChar = message.at(1);
    int messageIndex = 2;

    while(messageIndex < message.length() && !(previousChar == '\n' && currentChar == '\n')){
        previousChar = currentChar;
        currentChar = message.at(messageIndex);
        messageIndex++;
    }

    return message.substr(messageIndex, message.length() - messageIndex -1);
}
std::string Client::getMessageHeader(const std::string& message, const std::string& header) {
    std::stringstream messageStream(message);
    int len = header.length();
    std::string word, outputHeader = "";

    do{
        messageStream >> word;
    } while(word.compare(0, len, header) != 0);

    messageStream >> outputHeader;

    return outputHeader;
}
int Client::getReceiptID() {
    receiptID++;

    return receiptID;
}
std::string Client::intAsString(int someNumber) {
    std::stringstream stream;
    stream << someNumber;
    return stream.str();
}
void Client::registerAsReceiptPending(std::string outputStomp, int receiptID, std::string type) {

    pair<std::string, std::string> messageAndType(outputStomp, type);
    pair<int, pair<std::string, std::string>> toAdd(receiptID, messageAndType);

    pendingForReceipt->insert(toAdd);
}
void Client::setUserName(std::string userName) {
    this->userName = userName;
}
void Client::terminateNow() {
    terminate = true;
    messageQueue->push("ERROR");
    std::cout << "Disconnected. Exiting...\n" << std::endl;
}
int Client::getSubIdByGenre(std::string genre) {
    for (auto it = subscriptions->begin(); it != subscriptions->end(); ++it)
        if (it->second == genre)
            return it->first;
}
std::string Client::getBookName(std::string body, int startIndex, int endIndex) {

    std::vector<std::string> words;
    boost::split(words, body, boost::is_any_of(" "));

    std::string bookName = "";

    for(int i = startIndex; i < words.size() - endIndex; i++)
        bookName += words.at(i) +" ";

    bookName = bookName.substr(0, bookName.length()-1);

    return bookName;
}
std::string Client::getLastWord(std::string body) {
    std::vector<std::string> words;
    boost::split(words, body, boost::is_any_of(" "));

    return words.at(words.size()-1);
}

//Handle received frames
void Client::receiptReceived(std::stringstream *pstringstream, ConnectionHandler *connectionHandler) {
    std::string message = pstringstream->str();

    std::string receiptIDstring = getMessageHeader(message, "receipt-id");
    int receiptID = stoi(receiptIDstring);
    std::string originalMessage = pendingForReceipt->find(receiptID)->second.first;
    std::string messageType = pendingForReceipt->find(receiptID)->second.second;

    if(messageType == "logout") logoutReceipt(connectionHandler);
    else if(messageType == "exitGenre") exitGenreReceipt(originalMessage);
    else if(messageType == "joinGenre") joinGenreReceipt(originalMessage);
}
void Client::messageReceived(std::stringstream *pstringstream) {

    std::string messageBody = getMessageBody(pstringstream->str());
    std::string messageDestination = getMessageHeader(pstringstream->str(), "destination");
    int messageSubscription = stoi(getMessageHeader(pstringstream->str(), "subscription"));

    cout<<messageBody<<endl;

    if (messageBody == "book status") bookStatusReceived(messageDestination);
    else if (isWishToBorrow(messageBody)) wishToBorrowReceived(messageBody, messageDestination);
    else if (isSomeoneHasBook(messageBody)) someoneHasBookReceived(messageBody, messageDestination);
    else if (isSomeoneTakingBook(messageBody)) someoneTakingBookReceived(messageBody, messageDestination, messageSubscription);
    else if (isSomeoneReturningBook(messageBody)) someoneReturningBookReceived(messageBody, messageDestination);
}
void Client::errorReceived(stringstream *stringstream) {

    std::string nextWord, receiptID;

    do {
        *stringstream >> nextWord;
    } while(nextWord.compare("receipt-id:") != 0);

    *stringstream >> receiptID;

    do {
        *stringstream >> nextWord;
    } while(nextWord.compare("message:") != 0);

    std::string restOfMessage;
    getline(*stringstream, restOfMessage);

    int messageIndex = 2;
    char currentChar = restOfMessage.at(1);
    std::string errorMessage = "";
    errorMessage += currentChar;

    while(messageIndex < restOfMessage.length() && currentChar != '\n'){
        currentChar = restOfMessage.at(messageIndex);
        errorMessage = errorMessage +currentChar;
        messageIndex++;
    }

    cout << errorMessage << endl;
    terminateNow();
}
void Client::connectedReceived() {
    cout << "login successful" << endl;
}

//Handle keyboard input cases
void Client::logoutUser(ConnectionHandler *connectionHandler) {
    int receiptID = getReceiptID();

    std::string outputStomp =  "DISCONNECT\n"
                          "receipt: " +intAsString(receiptID) +
                          "\n"
                          "\0";

    registerAsReceiptPending(outputStomp, receiptID, "logout");
    messageQueue->push(outputStomp);
}
void Client::genreBookStatus(std::stringstream *stream) {
    std::string genre;
    *stream >> genre;

    std::string outputStomp = "SEND\n"
                         "destination: " +genre +"\n" +
                         "\n"
                         "book status\n"
                         "\0";

    messageQueue->push(outputStomp);
}
void Client::returnBook(std::stringstream *stream) {
    std::string genre, bookName, lender;

    *stream >> genre;

    getline(*stream, bookName);
    bookName = bookName.substr(1, bookName.length()-1);

    vector<tuple<std::string, std::string, bool>>* genreInventory = &bookInventory->find(genre)->second;

    for (auto currentBook = genreInventory->begin(); currentBook != genreInventory->end(); currentBook++)
        if(bookName.compare(get<0>(*currentBook)) == 0) {
            lender = get<1>(*currentBook);
            get<2>(*currentBook) = false;
            break;
        }

    std::string output = "SEND\n"
                    "destination: " +genre +"\n" +
                    "\n"
                    "Returning " +bookName +" to " +lender +"\n" +
                    "\0";

    messageQueue->push(output);
}
void Client::borrowBook(std::stringstream *stream) {
    std::string genre, bookName;

    *stream >> genre;
    getline(*stream, bookName);

    bookName = bookName.substr(1, bookName.length()-1);
    pendingBooks->push_back(bookName);

    std::string output =  "SEND\n"
                     "destination: " +genre +"\n" +
                     "\n" +
                     userName +" wish to borrow " +bookName +"\n" +
                     "\0";

    messageQueue->push(output);
}
void Client::addBook(std::stringstream *stream) {
    std::string genre, bookName;

    *stream >> genre;
    getline(*stream, bookName);
    bookName = bookName.substr(1, bookName.length()-1);

    if(bookInventory->find(genre) == bookInventory->end())
        bookInventory->emplace(genre, vector<tuple<std::string, std::string, bool>>());

    tuple<std::string, std::string, bool> book = make_tuple(bookName, userName, true);

    bookInventory->at(genre).push_back(book);

    std::string output =  "SEND\n"
                     "destination: " +genre +"\n" +
                     "\n" +
                     userName +" has added the book " +bookName +"\n" +
                     "\0";

    messageQueue->push(output);
}
void Client::exitGenre(std::stringstream *stream) {

    std::string genre;
    *stream >> genre;

    int subID = getSubIdByGenre(genre);
    int newReceiptID = getReceiptID();

    std::string outputStomp =  "UNSUBSCRIBE\n"
           "id: " +intAsString(subID) +"\n"
           "receipt: "+ intAsString(newReceiptID)+"\n"
           "\0";

    registerAsReceiptPending(outputStomp, newReceiptID, "exitGenre");
    messageQueue->push(outputStomp);
}
void Client::joinGenre(std::stringstream *stream) {
    std::string genre;
    *stream >> genre;

    int receiptID = getReceiptID();

    std::string outputStomp = "SUBSCRIBE\n"
           "destination: " +genre +"\n" +
           "id: "+ intAsString(subscriptionID) + "\n"
           "receipt: " +intAsString(receiptID) +"\n"
           "\0";
    subscriptionID++;

    registerAsReceiptPending(outputStomp, receiptID, "joinGenre");
    messageQueue->push(outputStomp);
}
void Client::login(std::stringstream *stream, ConnectionHandler *connectionHandler) {
    std::string hostPort, username, password;

    *stream >> hostPort;
    *stream >> username;
    *stream >> password;

    int devider = hostPort.find(':');
    std::string host = hostPort.substr(0, devider);
    short port = stoi(hostPort.substr(devider +1, hostPort.length() - devider));

    if (!connectionHandler->connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
    }
    else {
        cout << "Connected!" << endl;

        std::string output =  "CONNECT\n"
                         "accept-version: 1.2\n"
                         "host: stomp.cs.bgu.ac.il\n"
                         "login: " + username + "\n" +
                         "passcode: " + password + "\n" +
                         "\0";

        messageQueue->push(output);
    }
}

//Handle messages
void Client::bookStatusReceived(std::string destination) {
    if(bookInventory->find(destination) != bookInventory->end()){
        vector<tuple<std::string, std::string, bool>> *genreBooks = &bookInventory->find(destination)->second;

        bool haveBooks = false;
        std::string books = "";
        for(auto book : *genreBooks)
            if(get<2>(book)){
                books += get<0>(book) +", ";
                haveBooks = true;
            }

        if(haveBooks) {
            books = books.substr(0, books.length() - 2);

            std::string message = "SEND\n"
                             "destination: " + destination + "\n"
                                                             "\n" +
                             userName + ": " + books + "\n"
                                                       "\0";

            messageQueue->push(message);
        }
    }
}
void Client::wishToBorrowReceived(std::string body, std::string destination) {

    std::stringstream bodyStream(body);
    std::string otherUser, book;

    bodyStream >> otherUser;

    for(int i=1; i<=3; i++) bodyStream >> book;

    getline(bodyStream, book);
    book = book.substr(1, book.length()-1);

    vector<tuple<std::string, std::string, bool>> *genreInventory = &bookInventory->find(destination)->second;

    bool bookFound = false;
    for(auto currentBook : *genreInventory){
        if(book == get<0>(currentBook)){
            bookFound = true;
            break;
        }
    }

    if(bookFound){
        std::string message = "SEND\n"
                         "destination: " +destination +"\n" +
                         "\n"
                         +userName +" has " +book +"\n" +
                         "\0";

        messageQueue->push(message);
    }
}
void Client::someoneHasBookReceived(std::string body, std::string destination) {

    std::stringstream bodyStream(body);
    std::string otherUser, book;

    bodyStream >> otherUser >> book;

    getline(bodyStream, book);
    book = book.substr(1, book.length()-1);

    bool bookFound = false;
    for(std::string pendingBook : *pendingBooks){
        if(book == pendingBook){
            bookFound = true;
            break;
        }
    }

    if(bookFound){
        std::string message = "SEND\n"
                         "destination: " +destination +"\n" +
                         "\n"
                         "Taking " +book +" from " +otherUser +"\n" +
                         "\0";

        messageQueue->push(message);
    }
}
void Client::someoneTakingBookReceived(std::string body, std::string destination, int subscriptionId) {

    std::stringstream bodyStream(body);
    std::string book = "", lender, nextWord;

    bodyStream >> nextWord >> nextWord;

    while(nextWord != "from"){
        book += nextWord +" ";
        bodyStream >> nextWord;
    }

    book = book.substr(0, book.length()-1);
    bodyStream >> lender;

    //Case: we wanted the book. Assuming that only 1 client asks for a specific book.
    if (subscriptions->find(subscriptionId)->second == destination) {
        for (auto booksIter = pendingBooks->begin(); booksIter != pendingBooks->end(); booksIter++) {
            std::string currentBook = *booksIter;
            if (book.compare(currentBook) == 0) {
                pendingBooks->erase(booksIter);

                tuple<std::string, std::string, bool> toAdd(book, lender, true);
                bookInventory->find(destination)->second.push_back(toAdd);
                break;
            }
        }
    }

    //Case: someone is taking a book we have
    if (lender == userName){
        vector<tuple<std::string, std::string, bool>>* genreInventory = &bookInventory->find(destination)->second;

        for (auto currentBook = genreInventory->begin(); currentBook != genreInventory->end(); currentBook++){
            if(book.compare(get<0>(*currentBook)) == 0)
                get<2>(*currentBook) = false;
        }
    }
}
void Client::someoneReturningBookReceived(std::string body, std::string destination) {

    std::stringstream bodyStream(body);
    std::string lender, book;

    book = getBookName(body, 1, 2);
    lender = getLastWord(body);

    if(lender == userName) {
        vector<tuple<std::string, std::string, bool>>* genreInventory = &bookInventory->find(destination)->second;

        for (auto currentBook = genreInventory->begin(); currentBook != genreInventory->end(); currentBook++){
            if(book.compare(get<0>(*currentBook)) == 0)
                get<2>(*currentBook) = true;
        }
    }
}
bool Client::isWishToBorrow(std::string body) {

    std::stringstream bodyStream(body);
    std::string wish, to, borrow;

    bodyStream >> wish >> wish >> to >> borrow;

    return (wish == "wish" && to == "to" && borrow == "borrow");
}
bool Client::isSomeoneHasBook(std::string body) {

    std::stringstream bodyStream(body);
    std::string has;

    bodyStream >> has >> has;

    return has == "has";
}
bool Client::isSomeoneTakingBook(std::string body) {

    std::stringstream stream(body);
    std::string taking;

    stream >> taking;

    return taking == "Taking";
}
bool Client::isSomeoneReturningBook(std::string body) {

    std::stringstream stream(body);
    std::string returning;

    stream >> returning;

    return returning == "Returning";
}

//Handle receipts
void Client::logoutReceipt(ConnectionHandler *connectionHandler) {
    terminate = true;
    messageQueue->push("ERROR");
    connectionHandler->close();
}
void Client::exitGenreReceipt(std::string message) {
    int subscriptionIDToRemove = stoi(getMessageHeader(message, "id"));
    std::string genre = subscriptions->find(subscriptionIDToRemove)->second;

    subscriptions->erase(subscriptionIDToRemove);

    cout << "Exited club " +genre << endl;
}
void Client::joinGenreReceipt(std::string message) {

    int subscriptionIDToAdd = stoi(getMessageHeader(message, "id"));
    std::string genre = getMessageHeader(message, "destination");

    pair<int, std::string> newSubscription(subscriptionIDToAdd, genre);
    subscriptions->insert(newSubscription);

    vector<tuple<std::string, std::string, bool>> genreInventory;
    pair<std::string, vector<tuple<std::string, std::string, bool>>> toAdd = make_pair(genre, genreInventory);
    bookInventory->insert(toAdd);

    cout << "Joined club " +genre << endl;
}


































