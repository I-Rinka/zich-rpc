#include <tuple>
#include <iostream>
#include <string>

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

class Test
{
private:
    static size_t count;

public:
    void out_count()
    {
        count++;
        std::cout << this->count << std::endl;
    }
};

size_t Test::count = 0;

struct functor
{
    void operator()(int t1, std::string str)
    {
        std::cout << t1 << "," << str << std::endl;
    }
};

void function_pointer(double d, char c)
{
    std::cout << d << ' ' << c << std::endl;
}

int main(int argc, char const *argv[])
{
    Test t;
    auto lambda = [&](int a, double b, const char *c)
    {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
        t.out_count();
    };

    call(lambda, std::make_tuple(2333, 3.14, "Hello world!"));
    call(lambda, std::make_tuple(2, 3.99, "test2"));
    call(lambda, std::make_tuple(2, 4.5, "test3"));

    call(functor(), std::make_tuple(5, "test"));

    call(function_pointer, std::make_tuple(6.6, 'x'));

    return 0;
}

/**
 * 1. Get the type of all arguments
 * 2. Get the count of all arguments
 * 3. Serializer should perform exactly the amount of operations according to the binded function2
*/