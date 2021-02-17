#include <bits/stdc++.h>
#include <boost/asio.hpp>
using namespace boost::asio;

class socket_core
{
private:
    /* data */
    void BuildSession();
    char *host;
    char *port;
    ip::tcp::acceptor *acceptor_;
    ip::tcp::socket *socket_;

    void ServerAccept()
    {
        this->acceptor_->async_accept(
            [this](boost::system::error_code ec, ip::tcp::socket socket) {
                if (!ec)
                {
                    //do some thing
                    const char *host = socket.remote_endpoint().address().to_string().c_str();
                    int len = strnlen(host, 16);
                    this->host = (char *)calloc(len, sizeof(char));
                    strncpy(this->host, host, len);
                    // (*CallBackOnFirstAccept)(socket);
                }

                DoAccept();
            });
    }
    void DoAccept()
    {
        this->acceptor_->async_accept(
            [this](boost::system::error_code ec, ip::tcp::socket socket) {
                if (!ec)
                {
                    //do some thing
                    // (*CallBackOnAccept)(socket);
                    this->DoRead(socket);
                }

                DoAccept();
            });
    }
    void DoRead(ip::tcp::socket &socket)
    {

        async_read(socket, boost::asio::buffer(this->buffer, 6000), [&](boost::system::error_code ec, std::size_t) {
            if (!ec)
            {
                std::cout << this->buffer;
                std::cout << "\n";
                DoRead(socket);
            }
            else
            {
                socket.close();
            }
        });
    }
    void Dowrite()
    {
        boost::asio::async_write(socket_,
                                 boost::asio::buffer(write_msgs_.front().data(),
                                                     write_msgs_.front().length()),
                                 [this](boost::system::error_code ec, std::size_t /*length*/) {
                                     if (!ec)
                                     {
                                         write_msgs_.pop_front();
                                         if (!write_msgs_.empty())
                                         {
                                             do_write();
                                         }
                                     }

                                     else
                                     {
                                         socket_.close();
                                     }
                                 });
    }

public:
    void (*CallBackOnFirstAccept)(ip::tcp::socket &socket);
    void (*CallBackOnAccept)(ip::tcp::socket &socket);
    char *buffer;
    socket_core(const char *host, const char *port);
    socket_core(const char *port);
    ~socket_core();
    void Write(const char *msg)
    {
        boost::asio::post(io_context_,
                          [this, msg]() {
                              bool write_in_progress = !write_msgs_.empty();
                              write_msgs_.push_back(msg); //推入一个信息到队列
                              if (!write_in_progress)
                              {
                                  do_write(); //会把消息队列的开头读出去，然后用async_read把所有的信息都发送掉，然后关闭socket
                                              //async read：（socket，buffer，回调函数？（写完后执行？））//每写一次，都会命令后台线程调用do_write?
                              }
                          });
    }
};

socket_core::socket_core(const char *port)
{
    int len = strnlen(port, 6);
    this->port = (char *)calloc(len, sizeof(char));
    strncpy(this->port, port, len);

    io_context io_context;
    ip::tcp::endpoint endpoint(ip::tcp::v4(), std::atoi(host));

    this->acceptor_ = new ip::tcp::acceptor(io_context, endpoint);
    this->buffer = (char *)calloc(6000, sizeof(char));

    ServerAccept();
    std::thread t([&io_context]() { io_context.run(); });
}

socket_core::socket_core(const char *host, const char *port)
{
    /*初始化ip,端口*/
    int len = strnlen(host, 16);
    this->host = (char *)calloc(len, sizeof(char));
    strncpy(this->host, host, len);
    len = strnlen(port, 6);
    this->port = (char *)calloc(len, sizeof(char));
    strncpy(this->port, port, len);
    this->buffer = (char *)calloc(6000, sizeof(char));

    io_context io_context;
    ip::tcp::endpoint endpoint(ip::tcp::v4(), std::atoi(host));
    this->socket_ = new ip::tcp::socket(io_context);

    ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(host, port);

    boost::asio::connect(this->socket_, endpoints);//要不要异步?是否走无连接?
    // this->acceptor__ = new ip::tcp::acceptor(io_context, endpoint);
}

socket_core::~socket_core()
{
    free(this->buffer);
    // free(this->socket_);
    // free(this->)
}
