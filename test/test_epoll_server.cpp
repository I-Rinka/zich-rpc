#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <array>
#include <thread>

#include "../src/network/socket_connect.h"
using namespace std;

constexpr uint16_t PORT = 5030;
constexpr size_t MAX_EVENTS = 1000;

void server_thread()
{
    ServerSocket<> ss(PORT, 100);

    int epollfd = epoll_create(1);
    epoll_event event;
    event.data.fd = ss;
    event.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, ss, &event);

    while (true)
    {
        std::array<epoll_event, MAX_EVENTS> events;

        int event_num = epoll_wait(epollfd, events.begin(), events.size(), -1);

        if (event_num <= 0)
        {
            throw std::invalid_argument("epoll_wait() failed");
        }

        for (int i = 0; i < event_num; i++)
        {
            if ((events[i].data.fd == ss) && (events[i].events & EPOLLIN))
            {
                auto client = ss.accept();
                epoll_ctl(epollfd, EPOLL_CTL_ADD, client, &event);
            }
            else if (events[i].events & EPOLLIN)
            {
                auto temp_fd = events[i].data.fd;
                TCPSocket client(std::move(temp_fd)); // The socket will close after that!

                try
                {
                    auto data = client.recv();
                    if (data.size() == 0)
                    {
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, client, &event);
                    }
                    else
                    {
                        client.send("Echo: " + data);
                        // We need a mechanism to make client object not destruct
                    }
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, client, &event);
                }
            }
        }
    }
}

void client_thread()
{
    ClientSocket<> cs;
    cs.connect("127.0.0.1", PORT);

    string msg;

    cs.send("Hello!!!");

    try
    {
        auto recv = cs.recv();
        std::cout << recv << endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    // cs.close();
}

int main(int argc, char const *argv[])
{
    thread t(server_thread);
    // server_thread();

    this_thread::sleep_for(chrono::milliseconds(100));
    client_thread();

    t.join();
    return 0;
}
