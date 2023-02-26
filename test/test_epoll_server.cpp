#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <array>
#include <thread>
#include <vector>
#include <algorithm>

#include "../src/network/socket_connect.h"
#include "../src/network/length_prefixed_socket.h"
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

    TCPSocket client(-1);
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
                client = ss.accept();

                epoll_event event;
                event.data.fd = client;
                event.events = EPOLLIN;

                epoll_ctl(epollfd, EPOLL_CTL_ADD, client, &event);
            }
            else if (events[i].events & EPOLLIN)
            {

                auto temp_fd = events[i].data.fd;
                client = std::move(temp_fd);

                try
                {
                    auto data = client.recv();
                    cout << data << endl;
                    if (data.size() == 0)
                    {
                        epoll_event event;
                        event.events = EPOLLIN;
                        event.data.fd = client;
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, client, &event);
                    }
                    else
                    {
                        client.send("Echo: " + data);
                    }
                }
                catch (const std::exception &e)
                {
                    std::cerr << "server recv error" << endl;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, client, &event);
                    client.close();
                }
            }
        }
    }
    close(epollfd);
}

void client_thread()
{
    ClientSocket<> cs;

    if (cs.connect("127.0.0.1", PORT) == -1)
    {
        std::cout << "connect error" << endl;
        // return client_thread();
    }

    string msg;

    if (cs.send("Hello!!!") == 0)
    {
        cout << "sent error" << endl;
    }

    try
    {
        auto recv = cs.recv();
        std::cout << recv << endl;
        return client_thread();
    }
    catch (const std::exception &e)
    {
        std::cerr << "client receive error" << endl;
        // return client_thread();
    }

    // cs.close();
    return;
}

int main(int argc, char const *argv[])
{
    server_thread();
    return 0;
}
