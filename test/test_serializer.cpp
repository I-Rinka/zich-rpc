#include "TEST_SUIT.h"
#include <iostream>
#include "../src/packet/serializer.h"
#include "../src/util/function_traits.h"
using namespace std;

class MyDecoder : public Decoder
{
public:
    MyDecoder() = default;
    virtual long long DecodeNextInt() override
    {
        return 1;
    }
    virtual double DecodeNextDouble() override
    {
        return 3.14;
    }
    virtual bool DecodeNextBool() override
    {
        return true;
    }
    virtual std::string DecodeNextString() override
    {
        return "hello world!\n";
    }
    virtual bool ReachEnd() override
    {
        return false;
    }
    virtual bool ElementNumberEqual(size_t element) override
    {
        return true;
    }
    virtual void AddString(std::string &&packet) override
    {
    }
};

void my_func(int a, double b)
{
}

struct my_functor
{
    void operator()(double) {}
};

int main(int argc, char const *argv[])
{
    auto dc = MyDecoder();
    auto t = DecodePacket<function_traits<decltype(my_func)>::args_tuple>(dc);
    TEST((std::is_same<tuple_element<1, decltype(t)>::type, double>::value));

    auto lamb = [](std::string) {};
    auto t2 = DecodePacket<function_traits<decltype(lamb)>::args_tuple>(dc);
    TEST((std::is_same<tuple_element<0, decltype(t2)>::type, std::string>::value));

    auto t3 = DecodePacket<function_traits<decltype(my_functor())>::args_tuple>(dc);
    TEST((std::is_same<tuple_element<0, decltype(t3)>::type, double>::value));

    return 0;
}
