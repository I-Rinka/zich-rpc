#include "function_define.h"
#include "../src/zich_rpc.h"
#include <iostream>
using namespace std;

ServerStub ss(PORT);

double divide(double a, double b)
{
    return a / b;
}

void print_string(std::string str)
{
    cout << str << endl;
}

int counter = 0;
// later, original
std::tuple<int, int> add_count(int add)
{
    int before = counter;
    counter += add;
    return make_tuple(counter, before);
}

int main(int argc, char const *argv[])
{
    // ss.bind(RPCs::add_count, [](int add)
    //         { return add_count(add); });
    // ss.bind(RPCs::divide, [](double a, double b)
    //         { return divide(a, b); });

    // // ss.bind(RPCs::print_string, &print_string);
    // ss.bind(RPCs::print_string, [](string str)
    //         { print_string(str); });

    ss.bind(RPCs::add_count, add_count);
    ss.bind(RPCs::divide, divide);

    // ss.bind(RPCs::print_string, &print_string);
    ss.bind(RPCs::print_string, print_string);

    ss.start();
    return 0;
}
