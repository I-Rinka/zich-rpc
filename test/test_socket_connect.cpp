#include "../src/network/socket_connect.h"
#include "TEST_SUIT.h"
#include <iostream>
#include <thread>
#include <string>
#include <chrono>

using namespace std;
void process_request(ServerSocket &ss)
{
    auto client = ss.accept();

    while (true)
    {
        // client.send("Hello! World!\n");
        auto rcv = client.recv();
        std::cout << "server received: " << rcv << endl;

        client.send(string("echo from server!: ") + rcv);
    }

    process_request(ss);
}

static const uint16_t PORT = 5005;

void server_thread()
{
    std::cout << "Thread " << std::this_thread::get_id() << " created" << endl;

    ServerSocket ss(PORT);
    process_request(ss);
}

void client_thread()
{
    ClientSocket cs;
    cs.connect("127.0.0.1", PORT);

    string msg;

    while (true)
    {
        cin >> msg;
        cs.send(msg);

        this_thread::sleep_for(chrono::milliseconds(10));
        cout << "client received: " << cs.recv() << endl;
    }
}

int main(int argc, char **argv)
{
    thread t(server_thread);

    this_thread::sleep_for(chrono::milliseconds(100));
    client_thread();
    t.join();

    return 0;
}