#include <random>
#include <vector>
#include <stdint.h>
#include "../src/packet_protocal/simple_protocal.h"
#include "TEST_SUIT.h"
using namespace std;
#define TEST_SCALE 10000
#define ARG_SIZE 20

int main(int argc, char const *argv[])
{
    random_device rd;
    mt19937_64 seed(rd());
    uniform_int_distribution<> judge(0, 2);
    uniform_int_distribution<> strr('a', 'z');
    uniform_int_distribution<> strl(1, 10);
    uniform_int_distribution<> ll(INT32_MIN, INT32_MAX);
    uniform_int_distribution<> arity(1, ARG_SIZE);
    normal_distribution<> db(10, 1);

    for (int r = 0; r < TEST_SCALE; r++)
    {
        vector<Element> test_v;
        for (int i = 0; i < ARG_SIZE; i++)
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
                test_v.push_back(str);
                break;
                // int
            case 1:
                test_v.push_back((long long)ll(seed));
                break;
                // double
            case 2:
                test_v.push_back(db(seed));
                break;
            default:
                break;
            }
        }

        PacketBuilder pb;
        for (size_t i = 0; i < test_v.size(); i++)
        {
            switch (test_v[i].type)
            {
            case ElementType::STRING:
                pb.PushString(test_v[i].data_str);
                break;
            case ElementType::INT:
                pb.PushI64(test_v[i].int_val);
                break;
            case ElementType::FLOAT:
                pb.PushF64(test_v[i].float_val);
                break;
            default:
                break;
            }
        }
        auto v2 = ParsePack(pb.GetResult());
        if (!(test_v.size() == v2.size()))
        {
            TEST(false);
            return -1;
        }

        for (size_t i = 0; i < v2.size(); i++)
        {
            switch (v2[i].type)
            {
            case ElementType::STRING:
                if (!(v2[i].data_str == test_v[i].data_str))
                {
                    TEST(false);
                    std::cout << "before:" << test_v[i].data_str << ", after:" << v2[i].data_str << endl;
                    return -1;
                }

                break;
            case ElementType::INT:
                if (!(v2[i].int_val == test_v[i].int_val))
                {
                    TEST(false);
                    std::cout << "before:" << test_v[i].data_str << ", after:" << v2[i].data_str << endl;
                    return -1;
                }

                break;
            case ElementType::FLOAT:
                if ((fabs(v2[i].float_val - test_v[i].float_val) > 0.0001))
                {
                    TEST(false);
                    std::cout << "before:" << test_v[i].float_val << ", after:" << v2[i].float_val << endl;
                    return -1;
                }

                break;
            default:
                break;
            }
        }
    }
    TEST(true);
    return 0;
}
