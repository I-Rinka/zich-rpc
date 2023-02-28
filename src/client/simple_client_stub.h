#include "../network/socket_connect.h"
#include "../network/length_prefixed_socket.h"
#include <string>

class SClientStub
{
private:
    ClientSocket<LengthPrefixedSocket> *_cs;

public:
    SClientStub() : _cs(new ClientSocket<LengthPrefixedSocket>){};

    SClientStub(const SClientStub &) = delete;
    SClientStub &operator=(const SClientStub &) = delete;

    ~SClientStub()
    {
        if (_cs)
        {
            delete _cs;
        }
    }

    bool connect(std::string IP, uint16_t port)
    {
        return _cs->connect(IP, port);
    }

    // use as operation
    template <typename... Args>
    std::string call(std::string function_key, Args... args)
    {
        
    }
};