#include <iostream>
#include <unistd.h>
#include <memory>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <string.h>

class TCPSocket
{
protected:
    int _sockfd = -1;
    char *_buffer = nullptr;
    size_t _buffer_size = 0;

    const static size_t DEFAULT_BUFFER_SIZE = 1024;

public:
    std::string IP;
    uint16_t port = 0;

    TCPSocket();

    // Ownership stuff
    TCPSocket(TCPSocket &&other);
    TCPSocket(int &&sockfd);
    TCPSocket(const TCPSocket &) = delete;
    TCPSocket &operator=(const TCPSocket &) = delete;

    //! \brief Socket send data in the string
    //! \return The number of byte that is sent. Which means it might not send all of the chars in the string
    virtual size_t send(const std::string &data);

    //! \brief Socket receive data stored in the string
    //! \return std::string that stores data
    virtual std::string recv();

    virtual ~TCPSocket();

    //! \brief Make this class work with system api
    operator int() const;
};

class ClientSocket
{
protected:
    TCPSocket *_socket;

public:
    ClientSocket(const ClientSocket &) = delete;
    ClientSocket &operator=(const ClientSocket &) = delete;

    //! \brief Claim client socket without address
    //! \note If you call the default version, you need to call .connect after that
    ClientSocket() : _socket(new TCPSocket()){};
    ClientSocket(std::string ip, uint16_t port) : _socket(new TCPSocket()) { connect(ip, port); };

    //! \brief Connect to the target server
    bool connect(std::string ip, uint16_t port);

    //! \brief Socket send data in the string
    //! \return The number of byte that is sent. Which means it might not send all of the chars in the string
    virtual size_t send(const std::string &data) { return _socket->send(data); };

    //! \brief Socket receive data stored in the string
    //! \return std::string that stores data
    virtual std::string recv() { return _socket->recv(); };

    //! \brief This is close() semetics of socket
    virtual ~ClientSocket() = default;
};

class ServerSocket
{
protected:
    TCPSocket *_socket;

public:
    ServerSocket(const ServerSocket &) = delete;
    ServerSocket &operator=(const ServerSocket &) = delete;

    //! \brief Claim server socket without bind address
    //! \note If you call the default version, you need to call .bind(port) after that
    ServerSocket() : _socket(new TCPSocket()){};

    //! \brief Claim server socket and bind address
    ServerSocket(uint16_t port);

    //! \brief Call it if you use the default constructor.
    //! \return Return false if bind fails
    bool bind(uint16_t port);

    //! \brief Listen Sementics of socket
    //! \param connection_volume is the maxium acceptable client
    //! \return Return false if listen fails
    bool listen(int connection_volume = 100);

    //! \brief returns socket of connecting client
    //! \return TCPSocket of connecting client
    TCPSocket accept();

    //! \brief This is close() semetics of socket
    virtual ~ServerSocket() = default;

    //! \brief Socket send data in the string
    //! \return The number of byte that is sent. Which means it might not send all of the chars in the string
    virtual size_t send(const std::string &data) { return _socket->send(data); };

    //! \brief Socket receive data stored in the string
    //! \return std::string that stores data
    virtual std::string recv() { return _socket->recv(); };
};

// ---------------------------------------Implementation----------------------------------------

/**
 * @brief TCPSocket Class Implementation
 */

TCPSocket::TCPSocket()
{
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1)
    {
        throw std::runtime_error("Error creating socket");
    }

    _buffer = new char[DEFAULT_BUFFER_SIZE];
    _buffer_size = DEFAULT_BUFFER_SIZE;
};

TCPSocket::TCPSocket(TCPSocket &&other)
{
    _sockfd = other._sockfd;
    other._sockfd = -1;

    _buffer = other._buffer;
    _buffer_size = other._buffer_size;
    other._buffer_size = 0;
    other._buffer = nullptr;
};

// Ownership stuff
TCPSocket::TCPSocket(int &&sockfd)
{
    _sockfd = sockfd;

    _buffer = new char[DEFAULT_BUFFER_SIZE];
    _buffer_size = DEFAULT_BUFFER_SIZE;
}

TCPSocket::~TCPSocket()
{
    close(_sockfd);
    if (_buffer != nullptr)
    {
        delete _buffer;
    }
};

size_t TCPSocket::send(const std::string &data)
{
    if (_sockfd == -1)
    {
        throw std::runtime_error("Socket is not created before send");
    }

    int sent = ::send(_sockfd, data.c_str(), data.size(), 0);
    if (sent == -1)
    {
        throw std::runtime_error("Packet sent error");
    }
    return sent;
}

std::string TCPSocket::recv()
{
    if (_sockfd == -1)
    {
        throw std::runtime_error("Socket is not created before receive");
    }

    int recved = ::recv(_sockfd, _buffer, _buffer_size - 1, 0);
    if (recved == -1)
    {
        throw std::runtime_error("Packet sent error");
    }
    _buffer[recved] = 0;

    return std::string(_buffer);
}

TCPSocket::operator int() const
{
    return this->_sockfd;
}

/**
 * @brief ServerSocket Class Implementation
 */

ServerSocket::ServerSocket(uint16_t port) : _socket(new TCPSocket())
{
    this->bind(port);
}

bool ServerSocket::bind(uint16_t port)
{
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    _socket->IP = "0.0.0.0";
    _socket->port = port;

    int ret = ::bind(*_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    return ret == 0;
}

bool ServerSocket::listen(int connection_volume = 100)
{
    return ::listen(*_socket, connection_volume) == 0;
}

TCPSocket ServerSocket::accept()
{
    int socklen = sizeof(sockaddr_in);
    struct sockaddr_in client_addr;

    TCPSocket sock = ::accept(*_socket, (struct sockaddr *)&client_addr, (socklen_t *)&socklen);

    sock.port = client_addr.sin_port;

    char client_ip[INET_ADDRSTRLEN];

    // Get ip string from client
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    sock.IP = client_ip;

    return sock;
}

/**
 * @brief Client Socket Class Implementation
 */

bool ClientSocket::connect(std::string ip, uint16_t port)
{
    _socket->IP = ip;
    _socket->port = port;

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    int ret = ::connect(*_socket, (sockaddr *)(&server_addr), sizeof(server_addr));
    return ret != -1;
}