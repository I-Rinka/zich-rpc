#include <string>
#include <tuple>
#include <vector>
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

template <size_t N>
struct __decoder_helper
{
    template <typename F, typename... Args>
    static auto call(Decoder &Dc, F func, Args... args) -> typename function_traits<F>::args_tuple
    {
        using nth_type = typename function_traits<F>::template nth_type<N - 1>;
        if (Dc.ReachEnd())
        {
            throw std::runtime_error("Packet Integrity is not satisfied: parameters not enough");
        }

        return __decoder_helper<N - 1>::call(Dc, func, args..., __decoder<nth_type>::decode(Dc));
    }
};

template <>
struct __decoder_helper<0>
{
    template <typename F, typename... Args>
    static auto call(Decoder &Dc, F func, Args... args) -> typename function_traits<F>::args_tuple
    {

        return std::make_tuple(args...);
    }
};

struct ParameterDecoder
{
    template <typename F>
    static auto get(Decoder &Dc, F function) -> typename function_traits<F>::args_tuple
    {
        return __decoder_helper<function_traits<F>::arity>::call(Dc, function);
    }
};
#endif