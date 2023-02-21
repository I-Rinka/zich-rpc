#include <tuple>
#include <iostream>

template <typename OStream, int current, int tuple_size, typename... Args>
struct __print_t
{
    static void print(OStream &os, const std::tuple<Args...> &target)
    {
        os << std::get<current>(target) << (tuple_size == current + 1 ? "]" : ", ");
        __print_t<OStream, current + 1, tuple_size, Args...>::print(os, target);
    }
};

template <typename OStream, int tuple_size, typename... Args>
struct __print_t<OStream, tuple_size, tuple_size, Args...>
{
    static void print(OStream &os, const std::tuple<Args...> &target)
    {
    }
};

template <typename OStream, typename... Args>
OStream &print_tuple(OStream &os, const std::tuple<Args...> &target)
{
    os << "[";
    __print_t<OStream, 0, sizeof...(Args), Args...>::print(os, target);
    return os;
}

// int main(int argc, char const *argv[])
// {
//     auto t = std::make_tuple(1, "hello", 'b', -1, 3.14);
//     print_tuple(std::cout, t) << std::endl;

//     return 0;
// }
