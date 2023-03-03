#include <string>
#include <tuple>
#include "serializer.h"

template <int current, int total, typename... Args>
struct decoder_helper
{
    template <typename... Argc>
    static std::tuple<Args...> call(Decoder &Dc, Argc... args)
    {
        using nth_type = typename std::tuple_element<current, std::tuple<Args...>>::type;
        return decoder_helper<current + 1, total, Args...>::call(Dc, args..., __decoder<nth_type>::decode(Dc));
    }
};

template <int total, typename... Args>
struct decoder_helper<total, total, Args...>
{
    static std::tuple<Args...> call(Decoder &Dc, Args... args)
    {
        return std::make_tuple(args...);
    }
};

template <int current, int total, typename Tuple>
struct decoder_helper_tuple
{
    template <typename... Argc>
    static Tuple call(Decoder &Dc, Argc... args)
    {
        using nth_type = typename std::tuple_element<current, Tuple>::type;
        return decoder_helper_tuple<current + 1, total, Tuple>::call(Dc, args..., __decoder<nth_type>::decode(Dc));
    }
};

template <int total, typename Tuple>
struct decoder_helper_tuple<total, total, Tuple>
{
    template <typename... Args>
    static Tuple call(Decoder &Dc, Args... args)
    {
        return std::make_tuple(args...);
    }
};

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

    template <typename Tuple>
    Tuple as()
    {
        return decoder_helper_tuple<0, std::tuple_size<Tuple>::value, Tuple>::call(Dc);
    };

    template <typename... Args>
    std::tuple<Args...> as(std::tuple<Args...> const &tp)
    {
        return decoder_helper<0, sizeof...(Args), Args...>::call(Dc);
    }
};

template <typename _Decoder, typename _Encoder>
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
