#include <tuple>
#include <string>
#include <type_traits>
#include <iostream>
#include "../src/util/print_tuple.h"
using namespace std;

template <typename>
struct is_tuple : std::false_type
{
};

template <typename... T>
struct is_tuple<std::tuple<T...>> : std::true_type
{
};

template <bool use_tuple>
struct tuple_helper
{
    static void call()
    {
        cout << "Other" << endl;
    }
};

template <>
struct tuple_helper<true>
{
    static void call()
    {
        cout << "tuple" << endl;
    }
};

template <typename T>
void as()
{
    tuple_helper<is_tuple<T>::value>::call();
};

class MyClass
{
private:
    /* data */
public:
};

int main()
{
    auto tp = make_tuple(3.14, std::string("hello"));
    auto pr = make_pair(std::string("1"), 2);

    as<decltype(tp)>();
    as<decltype(pr)>();

    return 0;
}
