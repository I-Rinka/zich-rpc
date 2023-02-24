#include "TEST_SUIT.h"
#include <iostream>
#include "../src/packet/serializer.h"
using namespace std;

class MyDecoder : public Decoder
{
public:
    MyDecoder() = default;
    virtual int DecodeNextInt() override
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
};

void my_func(int a, int b)
{
}

int main(int argc, char const *argv[])
{
    cout << "hello world" << endl;
    auto dc = MyDecoder();
    auto t = ParameterDecoder::call(dc, my_func);
    return 0;
}
