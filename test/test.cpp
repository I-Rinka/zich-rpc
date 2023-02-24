#include <tuple>
#include <string>
#include <type_traits>

struct Decoder {
    bool DecodeNextBool() { return true; }
    double DecodeNextDouble() { return 3.14; }
    int DecodeNextInt() { return 42; }
    std::string DecodeNextString() { return "hello"; }
};

template <typename F>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R(Args...)>
{
    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N>
    struct nth_type
    {
        static_assert(N < arity, "error: invalid index.");
        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };
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
    template <typename F, typename... Args, typename... ArgT>
    static auto call(Decoder &Dc, F func, Args... args) -> std::tuple<ArgT...>
    {
        using nth_type = typename function_traits<F>::template nth_type<N - 1>::type;
        return __decoder_helper<N - 1>::template call<Dc, F, Args..., ArgT..., nth_type>(Dc, func, args..., __decoder<nth_type>::decode(Dc));
    }
};

template <>
struct __decoder_helper<0>
{
    template <typename F, typename... Args>
    static auto call(Decoder &, F func, Args... args) -> std::tuple<Args...>
    {
        return std::make_tuple(args...);
    }
};

struct ParameterDecoder
{
    template <typename F, typename... ArgT>
    static auto call(Decoder &Dc, F function) -> std::tuple<ArgT...>
    {
        return __decoder_helper<function_traits<F>::arity>::template call<Dc, F>(Dc, function);
    }
};

int main() {
    Decoder dc;
    auto tup = ParameterDecoder::call<decltype(std::make_tuple<int, double, std::string>), int, double, std::string>(dc, std::make_tuple<int, double, std::string>);
    return 0;
}
