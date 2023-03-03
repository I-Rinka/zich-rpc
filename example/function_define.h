#include <string>
#include <tuple>

constexpr uint16_t PORT = 7260;

enum class RPCs
{
    divide,
    print_string,
    add_count
};

double divide(double a, double b);

void print_string(std::string str);

// later, original
std::tuple<int, int> add_count(int add);