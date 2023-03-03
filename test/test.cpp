#include <tuple>
#include <string>
#include <type_traits>
#include <iostream>
#include "../src/util/print_tuple.h"
#include "../src/util/function_traits.h"
#include "../src/server/function_call_tule.h"
using namespace std;

template <typename F, typename... Args>
auto call(F func, Args... args) -> typename function_traits<F>::return_type
{
    return func(args...);
}

bool func_pointer(int a, long long b, double c)
{
    cout << a << endl;
    cout << b << endl;
    cout << c << endl;
    return true;
}

int main()
{
    // cout << call([](int a, double b)
    //              {
    //     std::cout << a << endl;
    //     cout << b << endl;
    //     return true; },
    //              1, 3.14);

    // cout << call(&func_pointer, 10);
    cout << call(func_pointer, 10, 2323, 3.14);
    cout << CallTuple(func_pointer, make_tuple(10, 2323, 3.14));

    return 0;
}
