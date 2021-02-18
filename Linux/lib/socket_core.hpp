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
                    // this->socket_ = (tcp::socket *)malloc(sizeof(tcp::socket));
                    // memcpy(this->socket_, &_socket, sizeof(tcp::socket));
                    this->socket_ = new tcp::socket(std::move(std::move(_socket)));
                    this->do_regist();
                    this->do_accept();
                }
                else
                {
                    // std::cout << ec << std::endl;
                    std::cout << ec.value() << std::endl;
                }
            });
    }
    void do_accept()
    {
        acceptor_->async_accept(
            [this](boost::system::error_code ec, tcp::socket _socket) {
                if (!ec)
                {
                    delete this->socket_;
                    this->socket_ = new tcp::socket(std::move(std::move(_socket)));
                    this->do_regist();
                }
                else
                {
                    // std::cout << ec << std::endl;
                    std::cout << ec.value() << std::endl;
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
                                        if (ec.value() == 2)
                                        {
                                            printf("断开连接");
                                        }

                                        std::cout << ec.value() << std::endl;
                                    }
                                });
    }

    void do_write()
    {
        boost::asio::async_write(*socket_,
                                 boost::asio::buffer(this->message, this->message_length),
                                 [this](boost::system::error_code ec, std::size_t /*length*/) {
                                     if (!ec)
                                     {
                                         //   do_write();
                                     }

                                     else
                                     {
                                         std::cout << ec.value() << std::endl;
                                     }
                                 });
    }

    void do_regist()
    {
        do_read();
    }

public:
    Socket_Core(const char *host, const char *port);
    Socket_Core(int port);
    ~Socket_Core();
    boost::asio::io_context *io_context_;
    tcp::socket *socket_;
    tcp::acceptor *acceptor_;
    char *message;
    int message_length = 200;

    void write(char *msg)
    {
        strncpy(this->message, msg, message_length);
        do_write();
    }
};

Socket_Core::Socket_Core(const char *host, const char *port)
{
    this->io_context_ = new boost::asio::io_context();

    tcp::resolver resolver = tcp::resolver(*this->io_context_);

    this->socket_ = new tcp::socket(*this->io_context_);

    this->message = (char *)malloc(message_length);

    boost::asio::connect(*this->socket_, resolver.resolve(host, port));
    do_regist();
    std::thread *t = new std::thread([this]() { this->io_context_->run(); });
}
Socket_Core::Socket_Core(int port)
{
    this->io_context_ = new boost::asio::io_context();

    this->acceptor_ = new tcp::acceptor(*this->io_context_, tcp::endpoint(tcp::v4(), port));
    this->message = (char *)malloc(message_length);
    first_accept();

    std::thread *t = new std::thread([this]() { this->io_context_->run(); });
}

Socket_Core::~Socket_Core()
{
}
