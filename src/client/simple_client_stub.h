#include "../network/socket_connect.h"
#include "../network/length_prefixed_socket.h"
#include "../packet/serializer.h"
#include "../packet/packet_parser.h"
#include <string>

class SEncoder : public Encoder
{
private:
    PacketBuilder pb;

public:
    virtual void EncodeBool(bool b) override
    {
        pb.PushI64(b);
    }
    virtual void EncodeDouble(double f) override
    {
        pb.PushF64(f);
    }
    virtual void EncodeInt(long long i) override
    {
        pb.PushI64(i);
    }
    virtual void EncodeString(std::string &str) override
    {
        pb.PushString(str);
    }
    virtual std::string GetResult() override
    {
        return pb.GetResult();
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
    template <typename... Args>
    std::string call(std::string function_key, Args... args)
    {
        SEncoder Ec;
        std::string pkt = EncodeParameters(Ec, args...);

        _cs->send(pkt);

        return _cs->recv();
    }

    template <typename... Args>
    std::string call(std::string IP, uint16_t port, std::string function_key, Args... args)
    {
        this->connect(IP, port);
        return call(function_key, std::forward<Args>(args...));
    }
};