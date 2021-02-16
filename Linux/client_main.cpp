#include <bits/stdc++.h>
#include <boost/asio.hpp>

using namespace boost::asio;
const char *host = "127.0.0.1";
const char *port = "1233";


int main(int argc, char const *argv[])
{
    io_context io;
    ip::tcp::resolver resolver(io);
    auto endpoints = resolver.resolve(host, port);
    ip::tcp::socket socket(io);

    connect(socket, endpoints);
    // int *content = (int *)malloc(sizeof(int) * 5000);
    // for (int i = 0; i < 5000; i++)
    // {
    //     content[i] = i;
    // }

    // socket.write_some(buffer(content, 5000 * sizeof(int)));

    socket.close();
    return 0;
}
