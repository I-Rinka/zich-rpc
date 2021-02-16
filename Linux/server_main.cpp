#include <bits/stdc++.h>
#include <boost/asio.hpp>

using namespace boost::asio;
const char *host = "127.0.0.1";
const char *port = "1233";
void loop(ip::tcp::acceptor *acceptor)
{
    acceptor->async_accept([=](const boost::system::error_code &error, ip::tcp::socket socket) {
        if (!error)
        {
            printf("fuck\n");
        }
        loop(acceptor);//相当于一直给acceptor注册任务
    });
}
int main(int argc, char const *argv[])
{
    io_context io;
    ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), atoi(port)));
    ip::tcp::socket socket(io);
    //lambda一般是函数参数的个数出了问题
    loop(&acceptor);
    io.run();
    return 0;
}
