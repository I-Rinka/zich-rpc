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
                    this->socket_ = new tcp::socket(std::move(_socket));
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
                    this->socket_ = new tcp::socket(std::move(_socket));
                    this->do_regist();
                }
                else
                {
                    // std::cout << ec << std::endl;
                    std::cout << ec.value() << std::endl;
                }
            });
    }

    void do_read_len()
    {
        boost::asio::async_read(*this->socket_, boost::asio::buffer(this->rcv_len, 10),
                                [this](boost::system::error_code ec, std::size_t /*length*/) {
                                    if (!ec)
                                    {
                                        int len = atoi(this->rcv_len);
                                        this->message_remain_to_read = len;

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
    void do_read()
    {

        boost::asio::async_read(*this->socket_, boost::asio::buffer(this->message_read, this->message_remain_to_read),
                                [this](boost::system::error_code ec, std::size_t /*length*/) {
                                    this->message_remain_to_read -= 4096;
                                    if (!ec)
                                    {
                                        if (this->message_remain_to_read > 0)
                                        {
                                            std::cout << this->message_remain_to_read << std::endl;
                                            this->message_remain_to_read += 4096;
                                            this->do_read();
                                        }
                                        else
                                        {
                                            this->message_read = this->message_read_head;
                                            memset(this->message_read, 0, INT32_MAX);
                                            this->message_remain_to_read = 0;
                                            this->do_read_len();
                                        }
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
    void do_write_len()
    {
        char msg_len[11];
        sprintf(msg_len, "%10d", this->message_remain_to_write);
        // boost::asio::write(*socket_, boost::asio::buffer(this->msg_len, 11));
        boost::asio::async_write(*socket_,
                                 boost::asio::buffer(msg_len, 10),
                                 [this](boost::system::error_code ec, std::size_t /*length*/) {
                                     if (!ec)
                                     {
                                         do_write();
                                     }

                                     else
                                     {
                                         std::cout << ec.value() << std::endl;
                                     }
                                 });
    }
    void do_write()
    {
        boost::asio::async_write(*socket_,
                                 boost::asio::buffer(this->message_write, this->message_remain_to_write),
                                 [this](boost::system::error_code ec, std::size_t /*length*/) {
                                     if (!ec)
                                     {
                                         this->message_remain_to_write -= 4096;
                                         if (this->message_remain_to_write > 0)
                                         {
                                             this->message_remain_to_write += 4096;
                                             do_write(); //继续写
                                         }
                                         else
                                         {
                                             this->message_remain_to_write = 0;
                                             this->message_write = this->message_write_head;
                                             memset(this->message_write, 0, INT32_MAX);
                                         }
                                     }

                                     else
                                     {
                                         std::cout << ec.value() << std::endl;
                                     }
                                 });
    }

    void do_regist()
    {
        do_read_header();
    }
    char *message_write_head = NULL;
    char *message_read_head = NULL;
    char rcv_len[11];

public:
    Socket_Core(const char *host, const char *port);
    Socket_Core(int port);
    ~Socket_Core();
    boost::asio::io_context *io_context_;
    tcp::socket *socket_;
    tcp::acceptor *acceptor_;

    char *message_write = NULL;
    char *message_read = NULL;
    int message_remain_to_write = 0;
    int message_remain_to_read = 0;

    void write(char *msg)
    {
        int len = strnlen(msg, INTMAX_MAX);
        this->message_remain_to_write = len;
        /*
        if (len > message_length)
        {
            if (message != NULL)
            {
                delete this->message;
            }
            this->message = (char *)malloc(len);
        }
        this->message_length = len;
        */
        strncpy(this->message_remain_to_write, msg, len);
        do_write_len();
    }
};

Socket_Core::Socket_Core(const char *host, const char *port)
{
    this->io_context_ = new boost::asio::io_context();
    this->message_read = (char *)malloc(INT32_MAX);
    this->message_write = (char *)malloc(INT32_MAX);
    this->message_write_head = this->message_write;
    this->message_read_head = this->message_read;

    tcp::resolver resolver = tcp::resolver(*this->io_context_);

    this->socket_ = new tcp::socket(*this->io_context_);

    boost::asio::connect(*this->socket_, resolver.resolve(host, port));

    do_regist();
    std::thread *t = new std::thread([this]() { this->io_context_->run(); });
}
Socket_Core::Socket_Core(int port)
{
    this->io_context_ = new boost::asio::io_context();
    this->message_read = (char *)malloc(INT32_MAX);
    this->message_write = (char *)malloc(INT32_MAX);
    this->message_write_head = this->message_write;
    this->message_read_head = this->message_read;

    this->acceptor_ = new tcp::acceptor(*this->io_context_, tcp::endpoint(tcp::v4(), port));
    first_accept();

    std::thread *t = new std::thread([this]() { this->io_context_->run(); });
}

Socket_Core::~Socket_Core()
{
}
