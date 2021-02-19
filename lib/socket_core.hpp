#include <bits/stdc++.h>
#include <boost/asio.hpp>
#include <functional>
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
                    this->socket_ = new tcp::socket(std::move(_socket));
                    this->do_regist();
                    this->do_accept();
                }
                else
                {
                    // std::cout << ec.value() << std::endl;
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
                    // std::cout << ec.value() << std::endl;
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

                                        // std::cout << ec.value() << std::endl;
                                    }
                                });
    }
    void do_read()
    {
        int len_to_read = this->one_socket_len;
        if (len_to_read > this->message_remain_to_read)
        {
            len_to_read = this->message_remain_to_read;
        }

        boost::asio::async_read(*this->socket_, boost::asio::buffer(this->message_read, len_to_read),
                                [this](boost::system::error_code ec, std::size_t /*length*/) {
                                    this->message_remain_to_read -= this->one_socket_len;

                                    if (!ec)
                                    {
                                        if (this->message_remain_to_read > 0)
                                        {

                                            this->message_read += this->one_socket_len;
                                            this->do_read();
                                        }
                                        else
                                        {
                                            this->message_read = this->message_read_head;
                                            // std::cout << this->message_read << std::endl;
                                            //读完了,在这里放回调,先别急着清空鸭
                                            // std::cout<<"为啥来了的消息不进回调?"<<std::endl;
                                            this->Recive_Callback(this->message_read);
                                            // std::cout<<"为啥来了的消息不进回调?"<<std::endl;
                                            memset(this->message_read, 0, this->max_len);
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
        boost::asio::async_write(*socket_, boost::asio::buffer(msg_len, 10),
                                 [this](boost::system::error_code ec, std::size_t /*length*/) {
                                     if (!ec)
                                     {
                                         do_write();
                                     }

                                     else
                                     {
                                        //  std::cout << ec.value() << std::endl;
                                     }
                                 });
    }
    void do_write()
    {
        int len_to_write = this->one_socket_len;
        if (len_to_write > this->message_remain_to_write)
        {
            len_to_write = this->message_remain_to_write;
        }

        boost::asio::async_write(*socket_,
                                 boost::asio::buffer(this->message_write, len_to_write),
                                 [this](boost::system::error_code ec, std::size_t /*length*/) {
                                     this->message_remain_to_write -= this->one_socket_len;
                                     if (!ec)
                                     {
                                         if (this->message_remain_to_write > 0)
                                         {
                                             this->message_write += this->one_socket_len;
                                             do_write(); //继续写
                                         }
                                         else
                                         {
                                             this->message_remain_to_write = 0;
                                             this->message_write = this->message_write_head;
                                             memset(this->message_write, 0, this->max_len);
                                         }
                                     }

                                     else
                                     {
                                        //  std::cout << ec.value() << std::endl;
                                     }
                                 });
    }

    void do_regist()
    {
        do_read_len();
    }
    char *message_write_head = NULL;
    char *message_read_head = NULL;
    char rcv_len[11];
    boost::asio::io_context *io_context_ = NULL;
    tcp::socket *socket_ = NULL;
    tcp::acceptor *acceptor_ = NULL;

    char *message_write = NULL;
    char *message_read = NULL;
    int message_remain_to_write = 0;
    int message_remain_to_read = 0;

public:
    const int max_len = 1000000;
    const int one_socket_len = 2048;
    Socket_Core(const char *host, const char *port);
    Socket_Core(int port);
    Socket_Core(const char *port);
    ~Socket_Core();
    std::function<void(const char *)> Recive_Callback;
    void RegistCallback( std::function<void(const char *)> Recive_Callback)
    {
        this->Recive_Callback=Recive_Callback;
    }
    void write(char *msg)
    {
        int len = strnlen(msg, max_len);
        this->message_remain_to_write = len;

        strncpy(this->message_write, msg, len);
        if (this->socket_ == NULL)
        {
            printf("NO Socket Available!");
        }
        else
        {
            do_write_len();
        }
    }
};

//客户端构造
Socket_Core::Socket_Core(const char *host, const char *port)
{
    this->io_context_ = new boost::asio::io_context();
    this->message_read = (char *)malloc(max_len);
    this->message_write = (char *)malloc(max_len);
    this->message_write_head = this->message_write;
    this->message_read_head = this->message_read;

    tcp::resolver resolver = tcp::resolver(*this->io_context_);

    this->socket_ = new tcp::socket(*this->io_context_);

    boost::asio::connect(*this->socket_, resolver.resolve(host, port));

    do_regist();
    std::thread *t = new std::thread([this]() { this->io_context_->run(); });
}

//服务器构造
Socket_Core::Socket_Core(int port)
{
    this->io_context_ = new boost::asio::io_context();
    this->message_read = (char *)malloc(max_len);
    this->message_write = (char *)malloc(max_len);
    this->message_write_head = this->message_write;
    this->message_read_head = this->message_read;

    this->acceptor_ = new tcp::acceptor(*this->io_context_, tcp::endpoint(tcp::v4(), port));
    first_accept();

    std::thread *t = new std::thread([this]() { this->io_context_->run(); });
}

Socket_Core::Socket_Core(const char*port):Socket_Core(atoi(port))
{

}

Socket_Core::~Socket_Core()
{
    //还没写析构
}
