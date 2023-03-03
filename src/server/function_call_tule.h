#include "../util/function_traits.h"
#include <tuple>

template <size_t N>
struct __call_helper
{
    template <typename Function, typename... Args>
    static auto call(Function F, typename function_traits<Function>::args_tuple tp, Args... args) -> typename function_traits<Function>::return_type
    {
        return __call_helper<N - 1>::call(F, tp, std::get<N - 1>(tp), args...);
    }
};

template <>
struct __call_helper<0>
{
    template <typename Function, typename... Args>
    static auto call(Function F, typename function_traits<Function>::args_tuple, Args... args) -> typename function_traits<Function>::return_type
    {
        // you can only pass functor / lambda to this function
        return F(args...);
    }
};

//! \brief you can only pass functor / lambda to this function
template <typename Function>
auto CallTuple(Function F, typename function_traits<Function>::args_tuple params_tp) -> typename function_traits<Function>::return_type
{
    return __call_helper<std::tuple_size<typename function_traits<Function>::args_tuple>::value>::call(F, params_tp);
}