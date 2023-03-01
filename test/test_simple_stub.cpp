#include "../src/server/simple_server_stub.h"
#include "../src/client/simple_client_stub.h"
#include <iostream>
#include <string>
#include <chrono>
#include "TEST_SUIT.h"
using namespace std;

constexpr uint16_t PORT = 6666;

enum class funcs
{
    func1,
    func2
};

void server_thread()
{
    SServerStub ss(PORT);

    ss.bind(funcs::func1, [=](int a, std::string b)
            { std::cout << "a is:" << a << endl; 
            std::cout << "b is:" << b << endl; });

    ss.start();
}

void client_thread(int num)
{
    SClientStub cs;
    cs.connect("127.0.0.1", PORT);
    for (int i = 0; i < 3; i++)
    {
        cs.call(funcs::func1, num, std::string("Hello world!"));
    }
}

int main(int argc, char const *argv[])
{
    thread t(server_thread);

    this_thread::sleep_for(chrono::milliseconds(5));

    for (int i = 0; i < 5; i++)
    {
        client_thread(i);
        this_thread::sleep_for(chrono::milliseconds(5));
    }

    t.join();

    return 0;
}
