all: server client threaded_client

server: server.cpp
	g++ $^ -o server.out -std=c++11 -lpthread

client: client.cpp
	g++ $^ -o client.out -std=c++11

threaded_client: threaded_client.cpp
	g++ $^ -o threaded_client.out -std=c++11 -lpthread