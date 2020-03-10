cmake_minimum_required(VERSION 3.15)
project(Client)

set(CMAKE_CXX_FLAGS "-g -Wall -Weffc++ -std=c++11 -pthread")
set(CMAKE_CXX_STANDARD 11)
    add_executable(Client src/Main.cpp src/ConnectionHandler.cpp src/Client.cpp src/ConcurrentQueue.cpp src/Readers.cpp include/ConcurrentQueue.h include/ConnectionHandler.h include/Readers.h include/Client.h )
find_package(Boost 1.56 REQUIRED COMPONENTS
        date_time filesystem iostreams thread)
target_link_libraries(Client Boost::date_time Boost::filesystem
        Boost::iostreams Boost::thread)

