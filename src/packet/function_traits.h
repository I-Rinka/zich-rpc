#include <type_traits>
#include <tuple>

template <typename F>
struct function_traits;

// Function pointer specialization
template <typename F, typename... Args>
struct function_traits<F (*)(Args...)>
{
    constexpr static size_t arity = sizeof...(Args);
    using return_type = F;
    template <size_t N>
    using nth_type = typename std::tuple_element<N, std::tuple<Args...>>::type;
};

// Function pointer specialization
template <typename F, typename... Args>
struct function_traits<F(Args...)>
{
    constexpr static size_t arity = sizeof...(Args);
    using return_type = F;
    template <size_t N>
    using nth_type = typename std::tuple_element<N, std::tuple<Args...>>::type;
};

// Functor const specialization
template <typename F, typename Class, typename... Args>
struct function_traits<F (Class::*)(Args...) const>
{
    constexpr static size_t arity = sizeof...(Args);
    using return_type = F;
    template <size_t N>
    using nth_type = typename std::tuple_element<N, std::tuple<Args...>>::type;
};

// Lambda specialization
template <typename L>
struct function_traits : public function_traits<decltype(&L::operator())>
{
};

// Functor specialization
template <typename F, typename Class, typename... Args>
struct function_traits<F (Class::*)(Args...)>
{
    constexpr static size_t arity = sizeof...(Args);
    using return_type = F;
    template <size_t N>
    using nth_type = typename std::tuple_element<N, std::tuple<Args...>>::type;
};

// template <typename F>
// struct function_arguments_to_tuple
// {
//     static
// };
