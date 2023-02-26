#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <array>
#include <thread>
#include <vector>
#include <algorithm>
#include <atomic>
#include <chrono>
#include "TEST_SUIT.h"

#include "../src/network/socket_connect.h"
#include "../src/network/length_prefixed_socket.h"
using namespace std;

constexpr uint16_t PORT = 5030;
constexpr size_t MAX_THREADS = 200;
std::atomic<int> current_thread(0);

void client_thread(int number)
{
    current_thread++;
    ClientSocket<> cs;
    if (!cs.connect("127.0.0.1", PORT))
    {
        std::cout << "connect error" << endl;
        // return client_thread();
    }

    // std::cout << number << ":" << current_thread << endl;

    if (cs.send(to_string(number) + GetRandomString(100)) == 0)
    {
        cout << "sent error" << endl;
    }

    try
    {
        auto recv = cs.recv();
        cout << recv << endl;
        // cout << number << endl;
        // std::cout << recv << endl;
        // return client_thread();
    }
    catch (const std::exception &e)
    {
        std::cerr << "client receive error," << number << ":" << current_thread << endl;
        // return client_thread();
    }

    // cs.close();
    // return client_thread(number);
    // cout << current_thread << endl;
    return;
}

int main(int argc, char const *argv[])
{
    // client_thread(1);

    auto start = std::chrono::high_resolution_clock().now();
    array<thread, MAX_THREADS> th_arr;
    int n = 0;
    auto generator = [n]() mutable
    { n++;
        return thread([n](){
            try
            {
            client_thread(n);
    
            }
            catch(const std::exception& e)
            {
                std::cerr << "thread gg" << endl;
            }
            
            }); };

    std::generate(th_arr.begin(), th_arr.end(), generator);

    for (auto &i : th_arr)
    {
        i.join();
    }

    auto duration = std::chrono::high_resolution_clock().now() - start;
    cout << MAX_THREADS << ":" << chrono::duration_cast<chrono::microseconds>(duration).count() << "ms" << endl;
    return 0;
}
