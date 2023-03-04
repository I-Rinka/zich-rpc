#include "../src/packet/serializer.h"
#include "../src/packet/netpacket_object.h"
#include "../src/packet_protocal/simple_protocal.h"
#include "../src/util/print_tuple.h"
#include "TEST_SUIT.h"
#include <tuple>
#include <vector>

using log_entry = std::tuple<size_t, size_t, std::string>;

template <>
struct __decoder<std::vector<log_entry>>
{
    static std::vector<log_entry> decode(Decoder &Dc)
    {
        std::string str = Dc.DecodeNextString();
        SDecoder sd(std::move(str));

        std::vector<log_entry> ans;

        while (!sd.ReachEnd())
        {
            size_t index = sd.DecodeNextInt();
            size_t term = sd.DecodeNextInt();
            std::string operation = sd.DecodeNextString();
            ans.emplace_back(std::make_tuple(index, term, operation));
        }

        return ans;
    }
};

template <>
struct __encoder<std::vector<log_entry>>
{
    static void encode(Encoder &Ec, std::vector<log_entry> &v)
    {
        SEncoder se;

        for (auto &i : v)
        {
            se.EncodeInt(std::get<0>(i));
            se.EncodeInt(std::get<1>(i));
            se.EncodeString(std::get<2>(i));
        }

        std::string ans = se.GetResult();
        Ec.EncodeString(ans);
    }
};

int main(int argc, char const *argv[])
{
    std::vector<log_entry> logs;
    for (int i = 0; i < 1000; i++)
    {
        logs.push_back(std::make_tuple(i, i / 4, GetRandomString()));
    }

    NetPacket<SEncoder, SDecoder> np;

    auto serialized_str = np.encode(logs);

    // std::cout << serialized_str << std::endl;

    auto ans = np.decode(serialized_str).as<std::vector<log_entry>>();

    for (size_t i = 0; i < logs.size(); i++)
    {
        TEST(logs[i] == ans[i]);
        // print_tuple(std::cout, ans[i]) << std::endl;
        // print_tuple(std::cout, logs[i]) << std::endl;
    }

    return 0;
}
