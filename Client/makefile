CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude -pthread -o
LDFLAGS:=-lboost_system -pthread

all: Client

Client: bin/Main.o bin/ConnectionHandler.o bin/Client.o bin/ConcurrentQueue.o bin/Readers.o

	g++ -o bin/Client bin/Main.o bin/Client.o bin/ConnectionHandler.o bin/ConcurrentQueue.o bin/Readers.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/Client.o: src/Client.cpp
	g++ $(CFLAGS) bin/Client.o src/Client.cpp

bin/ConcurrentQueue.o: src/ConcurrentQueue.cpp
	g++ $(CFLAGS) bin/ConcurrentQueue.o src/ConcurrentQueue.cpp

bin/Main.o: src/Main.cpp
	g++ $(CFLAGS) bin/Main.o src/Main.cpp

bin/Readers.o: src/Readers.cpp
	g++ $(CFLAGS) bin/Readers.o src/Readers.cpp


.PHONY: clean
clean:
	rm -f bin/*
