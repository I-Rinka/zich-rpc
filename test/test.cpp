#include <iostream>
#include <vector>

template <typename T>
struct function_traits;

template <typename ReturnType, typename... Args>
struct function_traits<ReturnType (*)(Args...)>
{
    enum
    {
        arity = sizeof...(Args)
    };
    typedef ReturnType result_type;

    template <size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const>
{
    enum
    {
        arity = sizeof...(Args)
    };
    typedef ReturnType result_type;

    template <size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())>
{
};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...)>
{
    enum
    {
        arity = sizeof...(Args)
    };
    typedef ReturnType result_type;

    template <size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

int my_function(int x, double y)
{
    return x + y;
}

struct test
{
    int operator()(int a, double b)
    {
        return 2;
    }
};

int main(int argc, char const *argv[])
{
    auto lambda = [](int a, double b) -> long long
    { return 3; };
    static_assert(std::is_same<long long, function_traits<decltype(lambda)>::result_type>::value, "err");
    static_assert(std::is_same<double, function_traits<decltype(lambda)>::arg<1>::type>::value, "err");

    std::cout << typeid(function_traits<decltype(lambda)>::result_type).name() << std::endl;

    using traits = function_traits<decltype(&my_function)>;

    std::cout << typeid(traits::result_type).name() << std::endl;

    // std::cout << test()(1, 2);
    std::cout << typeid(function_traits<test>::result_type).name() << std::endl;

    return 0;
}