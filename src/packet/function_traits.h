#include <type_traits>
#include <tuple>

template <typename F>
struct function_traits
{
    constexpr static size_t args_num = 0;
    using return_type = F;
};

template <typename F, typename T>
struct function_traits<F(T)>
{
    template <size_t N>
    struct _nth_type
    {
        using type = T;
    };

    constexpr static size_t args_num = 1;
    using return_type = F;

    template <std::size_t N>
    using nth_type = typename _nth_type<N>::type;
};

template <typename F, typename... Args>
struct function_traits<F(Args...)>
{
    template <size_t N>
    struct _nth_type
    {
        static_assert(N < sizeof...(Args), "Function arguments count does not match!");
        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };

    constexpr static size_t args_num = sizeof...(Args);

    using return_type = F;

    template <std::size_t N>
    using nth_type = typename _nth_type<N>::type;
};
