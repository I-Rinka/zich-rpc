#include "./socket_connect.h"
#include <limits>

#ifndef __Length_Prefixed_Socket_H__
#define __Length_Prefixed_Socket_H__
class LengthPrefixedSocket : public TCPSocket
{
protected:
    const static size_t DEFAULT_BUFFER_SIZE = 1 << 16;

public:
    LengthPrefixedSocket &operator=(int &&sockfd)
    {
        _sockfd = sockfd;
        return *this;
    }
    LengthPrefixedSocket &operator=(LengthPrefixedSocket &&other)
    {
        _sockfd = other._sockfd;
        other._sockfd = -1;

        IP = other.IP;
        port = other.port;

        if (other._buffer != nullptr)
        {
            if (_buffer == nullptr)
            {
                _buffer = other._buffer;
                _buffer_size = other._buffer_size;
                other._buffer = nullptr;
                other._buffer_size = 0;
            }
            else // This already has buffer
            {
                delete other._buffer;
                other._buffer_size = 0;
            }
        }

        return *this;
    }

    LengthPrefixedSocket(LengthPrefixedSocket &&other)
    {
        _sockfd = other._sockfd;
        other._sockfd = -1;

        IP = other.IP;
        port = other.port;

        _buffer = other._buffer;
        _buffer_size = other._buffer_size;
        other._buffer_size = 0;
        other._buffer = nullptr;
    };

    // Ownership stuff
    LengthPrefixedSocket(int &&sockfd)
    {
        _sockfd = sockfd;
    }

    LengthPrefixedSocket()
    {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_sockfd == -1)
        {
            throw std::runtime_error("Error creating socket");
        }
    }

    virtual size_t send(const std::string &data) override
    {
        if (_sockfd == -1)
        {
            throw std::runtime_error("Socket is not created before receive");
        }

        size_t length = data.size();
        if (::send(TCPSocket::_sockfd, &length, sizeof(length), MSG_NOSIGNAL) == -1)
        {
            throw std::runtime_error("Prefixed sent error");
        }

        const char *data_ptr = data.c_str();
        size_t bytes_sent = 0;
        while (bytes_sent < length)
        {
            int sent = ::send(TCPSocket::_sockfd, data_ptr + bytes_sent, length - bytes_sent, MSG_NOSIGNAL);
            if (sent == -1)
            {
                throw std::runtime_error("Packet sent error");
            }
            bytes_sent += sent;
        }

        return bytes_sent;
    }

    virtual std::string recv() override
    {
        if (_sockfd == -1)
        {
            throw std::runtime_error("Socket is not created before receive");
        }

        size_t length = 0; // If no data, the length will still become 0
        int recved = 0;
        if ((recved = ::recv(TCPSocket::_sockfd, &length, sizeof(length), 0)) == -1)
        {
            throw std::runtime_error("Prefixed length recv error");
        }

        // std::cout << "packet length:" << length << std::endl;
        // Client closed the connection
        // if (recved == 0)
        // {
        //     std::cout << "Bye" << length << std::endl;

        //     return "";
        // }

        if (_buffer == nullptr)
        {
            _buffer = new char[DEFAULT_BUFFER_SIZE];
            _buffer_size = DEFAULT_BUFFER_SIZE;
        }

        size_t bytes_received = 0;
        while (bytes_received < length)
        {
            ssize_t n = ::recv(TCPSocket::_sockfd, _buffer + bytes_received, length - bytes_received, 0);
            if (n == -1)
            {
                throw std::runtime_error("data recv error");
            }
            bytes_received += n;
        }

        _buffer[bytes_received] = 0;
        return std::string(_buffer);
    }
};
#endif