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
#include <map>
#include "TEST_SUIT.h"

#include "../src/network/socket_connect.h"
#include "../src/network/length_prefixed_socket.h"
using namespace std;

constexpr uint16_t PORT = 5030;
constexpr size_t MAX_THREADS = 1200;
std::atomic<int> current_thread(0);

void client_thread(int number)
{
    current_thread++;
    ClientSocket<LengthPrefixedSocket> cs;
    if (!cs.connect("127.0.0.1", PORT))
    {
        std::cout << "connect error" << endl;
        // return client_thread();
    }
    struct sockaddr_in local;
    socklen_t len = sizeof(local);
    getsockname(cs, (struct sockaddr *)&local, &len);
    // cout << "connected: " << ntohs(local.sin_port) << endl;

    // std::cout << number << ":" << current_thread << endl;

    if (cs.send(to_string(number) + GetRandomString_fx(5000)) == 0)
    {
        cout << "sent error" << endl;
    }

    try
    {
        auto recv = cs.recv();
        // cout << recv << endl;
        // cout << number << endl;
        // std::cout << recv << endl;
        // return client_thread();
        cout << "success: " << ntohs(local.sin_port) << endl;
    }
    catch (const std::exception &e)
    {
        return client_thread(number);
        std::cerr << "client receive error," << e.what() << number << ":";
        // return client_thread();
        cout << ntohs(local.sin_port) << endl;
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
