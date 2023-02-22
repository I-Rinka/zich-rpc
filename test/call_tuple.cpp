#include <tuple>
#include <iostream>

template <size_t remain_args>
struct __call_tuple
{
    template <typename functor, typename... ArgsT, typename... ArgsF>
    static void call(functor f, const std::tuple<ArgsT...> &tp, ArgsF... args)
    {
        return __call_tuple<remain_args - 1>::call(f, tp, std::get<remain_args - 1>(tp), std::forward<ArgsF>(args)...); // no template is needed
    }
};

template <>
struct __call_tuple<0>
{
    template <typename functor, typename... ArgsT, typename... ArgsF>
    static void call(functor f, const std::tuple<ArgsT...> &tp, ArgsF... args)
    {
        return f(std::forward<ArgsF>(args)...);
    }
};

template <typename functor, typename... ArgsT>
void call(functor f, const std::tuple<ArgsT...> &tp)
{
    __call_tuple<sizeof...(ArgsT)>::call(f, tp);
}

int main(int argc, char const *argv[])
{
    auto f = [](int a, double b, const char *c)
    {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
    };

    call(f, std::make_tuple(2333, 3.14, "Hello world!\n"));

    return 0;
}
