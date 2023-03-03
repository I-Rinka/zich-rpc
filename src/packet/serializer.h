#include <string>
#include <tuple>
#include <vector>
#include <iostream>
#include "function_traits.h"
// #include <iostream>

#ifndef _Zichrpc_SERIALIZER_H_
#define _Zichrpc_SERIALIZER_H_

//! \brief A pure virtual class for packet parser to inherit.
//! This is the adaptor to decode an upcoming string.
class Decoder
{
public:
    //! \brief The function of the decoder will be called for
    //! a certain amount of times defined by the binded function
    //! ReachEnd() can be used to check whether there still has
    //! remaining parameters in the string.
    virtual bool ReachEnd() = 0;
    //! \brief Because the number of times that the decoder parse
    //! the upcoming string is defined by the binded function, so
    //! ElementNumberEqual() should do an intergrity check for packet
    //! before performing the parser for an upcoming string.
    //! \param para_number the number of paramters of the binded function.
    virtual bool ElementNumberEqual(size_t para_number) = 0;

    //! \brief Add a new string, replace the old one for decode initialization
    virtual void AddString(std::string &&str) = 0;

    //! \brief Get the next value, which should be bool, of the upcoming string
    virtual bool DecodeNextBool() = 0;

    //! \brief Get the next value, which should be int, of the upcoming string
    virtual long long DecodeNextInt() = 0;

    //! \brief Get the next value, which should be double, of the upcoming string
    virtual double DecodeNextDouble() = 0;

    //! \brief Get the next value, which should be string, of the upcoming string
    virtual std::string DecodeNextString() = 0;

    // Todo decode object / array
};

class Encoder
{
public:
    virtual void EncodeBool(bool b) = 0;

    virtual void EncodeInt(long long i) = 0;

    virtual void EncodeDouble(double d) = 0;

    virtual void EncodeString(std::string &str) = 0;

    //! \brief Finally get the result
    virtual std::string GetResult() = 0;
};

template <typename T>
struct __decoder;

template <>
struct __decoder<bool>
{
    static bool decode(Decoder &Dc)
    {
        return Dc.DecodeNextBool();
    }
};

template <>
struct __decoder<double>
{
    static double decode(Decoder &Dc)
    {
        return Dc.DecodeNextDouble();
    }
};

template <>
struct __decoder<long long>
{
    static long long decode(Decoder &Dc)
    {
        return Dc.DecodeNextInt();
    }
};

template <>
struct __decoder<int>
{
    static int decode(Decoder &Dc)
    {
        return Dc.DecodeNextInt();
    }
};

template <>
struct __decoder<std::string>
{
    static std::string decode(Decoder &Dc)
    {
        return Dc.DecodeNextString();
    }
};

// template <size_t N, size_t current>
// struct __decoder_helper
// {
//     template <typename T, typename... Args>
//     static auto call(Decoder &Dc, T arg, Args... args) -> std::tuple<T, Args...>
//     {
//         if (Dc.ReachEnd())
//         {
//             throw std::runtime_error("Packet Integrity is not satisfied: parameters not enough");
//         }

//         return __decoder_helper<N, current + 1>::call(Dc, __decoder<T>::decode(Dc), args...);
//     }
// };

// template <size_t N>
// struct __decoder_helper<N, N>
// {
//     template <typename F, typename... Args>
//     static auto call(Decoder &Dc) -> typename function_traits<F>::args_tuple
//     {

//         return std::make_tuple(args...);
//     }
// };

// template <typename F>
// auto DecodeParameters(Decoder &Dc, F function) -> typename function_traits<F>::args_tuple
// {
//     return __decoder_helper<function_traits<F>::arity, 0>::call(Dc, function);
// }

template <typename T>
struct __encoder;

template <>
struct __encoder<bool>
{
    static void encode(Encoder &Ec, bool v)
    {
        Ec.EncodeBool(v);
    }
};

template <>
struct __encoder<double>
{
    static void encode(Encoder &Ec, double v)
    {
        Ec.EncodeDouble(v);
    }
};

template <>
struct __encoder<int>
{
    static void encode(Encoder &Ec, int v)
    {
        Ec.EncodeInt(v);
    }
};

template <>
struct __encoder<long long>
{
    static void encode(Encoder &Ec, long long v)
    {
        Ec.EncodeInt(v);
    }
};

template <>
struct __encoder<std::string>
{
    static void encode(Encoder &Ec, std::string &str)
    {
        Ec.EncodeString(str);
    }
};

template <size_t N>
struct __encoder_helper
{
    template <typename T, typename... Args>
    static std::string call(Encoder &Ec, T v, Args... args)
    {
        __encoder<T>::encode(Ec, v);
        return __encoder_helper<N - 1>::call(Ec, args...);
    }
};

template <>
struct __encoder_helper<0>
{
    static std::string call(Encoder &Ec)
    {
        return Ec.GetResult();
    }
};

template <typename... Args>
std::string EncodeParameters(Encoder &Ec, Args... args)
{
    return __encoder_helper<sizeof...(args)>::call(Ec, args...);
}

template <int current, int total, typename... Args>
struct __encode_tuple_helper
{
    static std::string call(Encoder &Ec, std::tuple<Args...> &tp)
    {
        using nth_type = typename std::tuple_element<current, std::tuple<Args...>>::type;
        __encoder<nth_type>::encode(Ec, std::get<current>(tp));
        return __encode_tuple_helper<current + 1, total, Args...>::call(Ec, tp);
    }
};

// end recursion
template <int total, typename... Args>
struct __encode_tuple_helper<total, total, Args...>
{
    static std::string call(Encoder &Ec, std::tuple<Args...> &tp)
    {
        return Ec.GetResult();
    }
};

template <typename... Args>
std::string EncodeTuple(Encoder &Ec, std::tuple<Args...> &tp)
{
    return __encode_tuple_helper<0, sizeof...(Args), Args...>::call(Ec, tp);
}

#endif