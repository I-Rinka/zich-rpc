#include "../src/network/socket_connect.h"
#include "TEST_SUIT.h"
#include <iostream>
#include <thread>
#include <string>
#include <string.h>
using namespace std;
void process_request(ServerSocket &ss)
{
    auto client = ss.accept();

    while (true)
    {
        auto rcv = client.recv();
        if (rcv.size() == 0)
        {
            break;
        }
        std::cout << rcv << endl;

        client.send(string("echo from server!: ") + rcv);
    }

    process_request(ss);
}

#define PORT 6666

int server_thread()
{
    ServerSocket ss(PORT);
    process_request(ss);
    return 0;
}

int main(int argc, char **argv)
{
    std::thread(server_thread);

    ClientSocket cs;
    cs.connect("127.0.0.1", PORT);

    string msg;
    while (true)
    {
        cin >> msg;
        cs.send(msg);

        cout << cs.recv();
    }

    return 0;
}