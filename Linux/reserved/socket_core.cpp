#include <bits/stdc++.h>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
class Socket_Core
{
private:
    /* data */
    void first_accept()
    {
        acceptor_->async_accept(
            [this](boost::system::error_code ec, tcp::socket _socket) {
                if (!ec)
                {
                    this->socket_ = (tcp::socket *)malloc(sizeof(tcp::socket));
                    memccpy(this->socket_, &socket, 1, sizeof(tcp::socket));
                }
            });
    }

    void do_read()
    {
        boost::asio::async_read(*this->socket_, boost::asio::buffer(this->message, this->message_length),
                                [this](boost::system::error_code ec, std::size_t /*length*/) {
                                    if (!ec)
                                    {
                                        std::cout << this->message << std::endl;
                                        this->do_read();
                                    }
                                    else
                                    {
                                    }
                                });
    }

    void do_regist()
    {
        do_read();
    }

public:
    Socket_Core(const char *host, const char *port);
    Socket_Core(tcp::acceptor *acceptor);
    ~Socket_Core();
    boost::asio::io_context *io_context_;
    tcp::socket *socket_;
    tcp::acceptor *acceptor_;
    char *message;
    int message_length;

    void write(char *msg)
    {
        strncpy(this->message, msg, message_length);
        boost::asio::write(*this->socket_, boost::asio::buffer(message, message_length));
    }
};

Socket_Core::Socket_Core(const char *host, const char *port)
{
    this->io_context_ = new boost::asio::io_context();

    tcp::resolver resolver = tcp::resolver(*this->io_context_);

    this->socket_ = new tcp::socket(*this->io_context_);

    boost::asio::connect(*this->socket_, resolver.resolve(host, port));
    do_regist();
    std::thread([this]() { this->io_context_->run(); });
}
Socket_Core::Socket_Core(tcp::acceptor *acceptor)
{
    this->io_context_ = new boost::asio::io_context();

    this->acceptor_ = acceptor;
    this->message = (char *)malloc(message_length);
    do_regist();
    first_accept();

    std::thread([this]() { this->io_context_->run(); });
}

Socket_Core::~Socket_Core()
{
}
