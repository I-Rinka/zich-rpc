#include <string>
#include <tuple>
#include <iostream>
#include <array>
#include "./convert_enum.h"
#include "print_tuple.h"

template <typename T, typename... Args>
void call(T func_key, Args... args)
{
    std::string key = get_str_rep(func_key);
    int seq = 1;
    auto caller_object = tuple<string, int, Args...>(key, seq, args...);

    print_tuple(std::cout, caller_object);
}

enum class funcs
{
    func1,
    func2,
    func3
};

int main(int argc, char const *argv[])
{
    call(funcs::func2, 1, 3.14, "hello");
    return 0;
}
