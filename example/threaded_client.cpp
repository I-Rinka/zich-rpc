#include "function_define.h"
#include "../src/zich_rpc.h"
#include <iostream>
#include <thread>
#include <vector>
using namespace std;

void client_request()
{
    ClientStub cs;
    cs.connect("127.0.0.1", PORT);

    cs.call(RPCs::print_string, "hello server!");
    cs.call(RPCs::print_string, "I am client");

    // call function divide, get return value as double
    cout << "Call divde 10 / 3.14 = ";
    cout << cs.call(RPCs::divide, 10, 3.14).as<double>() << endl; // Cpp has deduction problem. 10.0 must be 10.0 instead of 10. Otherwize it will become int

    for (int i = 0; i < 10; i++)
    {
        // function returns two value as a tuple
        using return_type = function_traits<decltype(add_count)>::return_type;
        auto tp = cs.call(RPCs::add_count, i).as<return_type>();

        cout << "now is: " << std::get<0>(tp) << ", last time was:" << std::get<1>(tp) << endl;
    }
}

int main(int argc, char const *argv[])
{
    vector<thread> v;
    for (int i = 0; i < 100; i++)
    {
        v.emplace_back(thread(client_request));
    }
    for (auto &i : v)
    {
        i.join();
    }

    return 0;
}
