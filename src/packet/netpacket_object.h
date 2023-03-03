#include <tuple>
#include <string>
#include "serializer.h"
#ifndef __NETPACKET_OBJ_H__
#define __NETPACKET_OBJ_H__
template <typename _Decoder>
struct decode_obj
{
private:
    _Decoder Dc;

public:
    decode_obj(std::string &&str)
    {
        Dc.AddString(std::move(str));   
    };

    template <typename T>
    T as()
    {
        return DecodePacket<T>(Dc);
    };
};

template <typename _Encoder, typename _Decoder>
class NetPacket
{
public:
    decode_obj<_Decoder> decode(std::string &&str)
    {
        return decode_obj<_Decoder>(std::move(str));
    }

    decode_obj<_Decoder> decode(std::string &str)
    {
        std::string t_str = str;
        return decode_obj<_Decoder>(std::move(t_str));
    }

    template <typename... Args>
    std::string encode(Args... args)
    {
        _Encoder EC;
        return EncodeParameters(EC, args...);
    }

    template <typename... Args>
    std::string encode(std::tuple<Args...> &tp)
    {
        _Encoder EC;
        return EncodeTuple(EC, tp);
    }
};
#endif