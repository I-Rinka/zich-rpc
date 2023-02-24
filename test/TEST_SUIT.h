#define TEST(t)                                                                                                                                          \
    (bool)(t);                                                                                                                                           \
    if (!(t))                                                                                                                                            \
    {                                                                                                                                                    \
        std::cout << "\033[1;31m[Failed]\033[0m at " << __FILE__ << ":" << __LINE__ << " in function \033[1;33m" << __FUNCTION__ << "()\033[0m" << endl; \
    }                                                                                                                                                    \
    else                                                                                                                                                 \
    {                                                                                                                                                    \
        std::cout << "\033[1;32m[Pass]\033[0m " << __FILE__ << ":" << __LINE__ << endl;                                                                  \
    }

#include <random>
#include <string>
using namespace std;

string GetRandomString(int max_len = 15)
{
    string ans;
    random_device rd;
    mt19937_64 seed(rd());
    uniform_int_distribution<> strc('a', 'z');
    uniform_int_distribution<> strl(1, max_len);

    int l = strl(seed);
    for (int i = 0; i < l; i++)
    {
        ans.push_back(strc(seed));
    }
    return ans;
}