#include "../util/get_str_rep.h"
#include "../packet/netpacket_object.h"
#include "../packet_protocal/simple_protocal.h"
#include "../network/socket_connect.h"
#include "../network/length_prefixed_socket.h"

#include <string>
#include <functional>

#ifndef __SIMPLE_CLIENT_STUB_H__
#define __SIMPLE_CLIENT_STUB_H__
class RETURN : private NetPacket<SEncoder, SDecoder>
{
private:
    std::function<std::string()> net_function;

public:
    RETURN() = delete;
    RETURN(std::function<std::string()> f) : NetPacket<SEncoder, SDecoder>(), net_function(f){};

    template <typename T>
    T as()
    {
        return NetPacket<SEncoder, SDecoder>::decode(net_function()).as<T>();
    }
};

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
    template <typename Key, typename... Args>
    RETURN call(Key function_key, Args... args)
    {
        SEncoder Ec;
        std::string pkt = EncodeParameters(Ec, GetStrRep(function_key), args...);

        _cs->send(pkt);

        // Design: make nettraffic delay accept
        // if the function is void, the client will not be block without calling .as
        // if the function has return value, the client will be blocked to receive network packet
        return RETURN([&]() -> std::string
                      { return _cs->recv(); });
    }
};
#endif