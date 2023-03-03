#include "../src/packet/netpacket_object.h"
#include "../src/packet_protocal/simple_protocal.h"
#include "TEST_SUIT.h"
#include <tuple>
#include <iostream>
#include "../src/util/print_tuple.h"

using namespace std;

class SimpeNetPacket : public NetPacket<SDecoder, SEncoder>
{
};

int main(int argc, char const *argv[])
{
    SimpeNetPacket sp;
    auto tp = make_tuple(1, std::string("hello"), 3.14, true, false, std::string("world!"));

    auto str = sp.encode(tp);

    auto tp2 = sp.decode(str).as<decltype(tp)>();

    print_tuple(cout, tp2) << endl;

    auto tp3 = sp.decode(str).as(tp2);

    print_tuple(cout, tp3);

    return 0;
}
