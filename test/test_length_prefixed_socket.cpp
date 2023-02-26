#include "../src/network/length_prefixed_socket.h"
#include "../src/network/socket_connect.h"
#include "TEST_SUIT.h"
#include <iostream>
#include <thread>
#include <string>
#include <chrono>

using namespace std;

#define TEST_SCALE 10000
#define PACK_SIZE ((1 << 16) - 1)
static const uint16_t PORT = 5002;

vector<string> test_v;

void process_request(ServerSocket<LengthPrefixedSocket> &ss)
{
    auto client = ss.accept();

    int round = 0;
    for (int i = 0; i < test_v.size(); i++)
    {
        auto rcv = client.recv();
        client.send(rcv);
    }
    auto rcv = client.recv(); // There should be a last recv for get close from client
    std::cout << "client said good bye:" << rcv.size() << std::endl;
}

void server_thread()
{
    std::cout << "Thread " << std::this_thread::get_id() << " created" << endl;

    ServerSocket<LengthPrefixedSocket> ss(PORT);
    process_request(ss);
    // this_thread::sleep_for(chrono::milliseconds(500));
}

void client_thread()
{
    ClientSocket<LengthPrefixedSocket> cs;
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
        // test_v.push_back(GetRandomString_fx(PACK_SIZE));
        test_v.push_back(GetRandomString(PACK_SIZE));
    }
}

int main(int argc, char **argv)
{
    generate_test_case();

    thread t(server_thread);
    // server_thread();

    this_thread::sleep_for(chrono::milliseconds(100));
    client_thread();

    t.join();
    return 0;
}