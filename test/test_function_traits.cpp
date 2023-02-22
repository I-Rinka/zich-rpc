#include "../src/packet/function_traits.h"
#include "TEST_SUIT.h"
#include <iostream>
#include <type_traits>
#include <string>
#include <functional>
#include <vector>

#include <assert.h> // can't use assert for template!
using namespace std;

void pointer_func(int a, double b, string str)
{
}

int main(int argc, char const *argv[])
{
    // function pointer test
    TEST((std::is_same<void, function_traits<decltype(pointer_func)>::return_type>::value));
    TEST((std::is_same<double, function_traits<decltype(pointer_func)>::nth_type<1>>::value));
    TEST((std::is_same<string, function_traits<decltype(pointer_func)>::nth_type<2>>::value));
    TEST(function_traits<decltype(pointer_func)>::args_num == 3);

    auto lamb = [](int &&a, double &b) -> vector<int>
    {
        return {};
    };

    TEST((std::is_same<vector<int>, function_traits<decltype(lamb)>::return_type>::value)); // Directly test lambda is broken
    // TEST((std::is_same<vector<int>, function_traits<decltype(lamb)>::return_type>::value));
    std::function<vector<int>(int &&, double &)> f(lamb);
    std::cout << function_traits<decltype(f)>::args_num << endl;

    // TEST((std::is_same<int&&, function_traits<decltype(lamb)>::nth_type<0>::type>::value));
    // TEST((std::is_same<double&, function_traits<decltype(lamb)>::nth_type<1>::type>::value));

    return 0;
}
