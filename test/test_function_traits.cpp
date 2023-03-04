#include "../src/util/function_traits.h"
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

class my_functor
{
public:
    vector<int> operator()(long long a, vector<string> b)
    {
        return {};
    }
};

int main(int argc, char const *argv[])
{
    TEST((std::is_same<void, function_traits<decltype(&pointer_func)>::return_type>::value));
    TEST((std::is_same<std::string, function_traits<decltype(&pointer_func)>::nth_type<2>>::value));
    TEST((function_traits<decltype(&pointer_func)>::arity == 3));
    TEST((std::is_same<void, function_traits<decltype(pointer_func)>::return_type>::value));
    TEST((std::is_same<std::string, function_traits<decltype(pointer_func)>::nth_type<2>>::value));
    TEST((function_traits<decltype(pointer_func)>::arity == 3));
    cout << "function pointer test OK" << endl;

    TEST((std::is_same<vector<int>, function_traits<decltype(&my_functor::operator())>::return_type>::value)); // There are two ways to pass functor
    TEST((std::is_same<long long, function_traits<my_functor>::nth_type<0>>::value));
    TEST((function_traits<my_functor>::arity == 2));
    cout << "functor test OK" << endl;

    auto lamb = [](double d) -> long long
    { return 2; };
    TEST((std::is_same<long long, function_traits<decltype(lamb)>::return_type>::value));
    TEST((std::is_same<double, function_traits<decltype(lamb)>::nth_type<0>>::value));
    TEST((function_traits<decltype(lamb)>::arity == 1));
    cout << "lambda test OK" << endl;

    return 0;
}
