#include <random>
#include <vector>
#include "../src/packet/packet_parser.h"
#include "TEST_SUIT.h"
using namespace std;
#define TEST_SCALE 10000

int main(int argc, char const *argv[])
{
    random_device rd;
    mt19937_64 seed(rd());
    uniform_int_distribution<> judge(0, 2);
    uniform_int_distribution<> strr('a', 'z');
    uniform_int_distribution<> strl(1, 10);
    uniform_int_distribution<> ll(INT_MIN, INT_MAX);
    normal_distribution<> db(10, 0.2);

    for (int r = 0; r < 100; r++)
    {
        vector<Element> v;
        for (int i = 0; i < TEST_SCALE; i++)
        {
            string str;
            int l = strl(seed);
            switch ((int)judge(seed))
            {
                // string
            case 0:
                for (int j = 0; j < l; j++)
                {
                    str.push_back(strr(seed));
                    if (strl(seed) % judge(seed) == 0)
                    {
                        str.push_back('\n');
                    }
                }
                v.push_back(str);
                break;
                // int
            case 1:
                v.push_back((long long)ll(seed));
                break;
                // double
            case 2:
                v.push_back(db(seed));
                break;
            default:
                break;
            }
        }

        PacketBuilder pb;
        for (size_t i = 0; i < v.size(); i++)
        {
            switch (v[i].type)
            {
            case ElementType::STRING:
                pb.PushString(v[i].data_str);
                break;
            case ElementType::INT:
                pb.PushI64(v[i].int_val);
                break;
            case ElementType::FLOAT:
                pb.PushI64(v[i].float_val);
                break;
            default:
                break;
            }
        }

        auto v2 = ParsePack(pb.GetResult());
        auto rs = TEST(v.size() == v2.size());
        if (!rs)
        {
            return -1;
        }

        for (size_t i = 0; i < v2.size(); i++)
        {
            switch (v2[i].type)
            {
            case ElementType::STRING:
                if (!(v2[i].data_str == v[i].data_str))
                {
                    TEST(false);
                    std::cout << "before:" << v[i].data_str << ", after:" << v2[i].data_str << endl;
                }

                break;
            case ElementType::INT:
                if (!(v2[i].int_val == v[i].int_val))
                {
                    TEST(false);
                    std::cout << "before:" << v[i].data_str << ", after:" << v2[i].data_str << endl;
                }

                break;
            case ElementType::FLOAT:
                if (!(fabs(v2[i].float_val - v[i].float_val) > 0.0001))
                {
                    TEST(false);
                    std::cout << "before:" << v[i].data_str << ", after:" << v2[i].data_str << endl;
                }

                break;
            default:
                break;
            }
        }
    }

    return 0;
}
