#include "../src/network/socket_connect.h"
#include "TEST_SUIT.h"
#include <iostream>
#include <thread>
#include <string>
#include <chrono>

using namespace std;

#define TEST_SCALE 10
#define PACK_SIZE 1000

vector<string> test_v;

void process_request(ServerSocket &ss)
{
    auto client = ss.accept();

    int round = 0;
    for (int i = 0; i < test_v.size(); i++)
    {
        auto rcv = client.recv();
        client.send(rcv);
    }
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

    for (int i = 0; i < TEST_SCALE; i++)
    {
        cs.send(test_v[i]);
        auto recv = cs.recv();

        bool ok = TEST((recv == test_v[i]));
        if (!ok)
        {
            std::cout << "expected: " << test_v[i] << ", received: " << recv << endl;
            this_thread::sleep_for(chrono::milliseconds(300));
        }
    }
    cs.close();
}

void generate_test_case()
{
    for (int i = 0; i < TEST_SCALE; i++)
    {
        test_v.push_back(GetRandomString(PACK_SIZE));
    }
}

int main(int argc, char **argv)
{
    generate_test_case();

    thread t(server_thread);

    this_thread::sleep_for(chrono::milliseconds(100));
    client_thread();

    t.join();
    return 0;
}