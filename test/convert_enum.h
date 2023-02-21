#include <iostream>
#include <type_traits>
using namespace std;

// enum class funcs
// {
//     apple,
//     pear,
//     orange
// };

template <typename T, bool is_enum>
struct get_str
{
    static string convert(T value)
    {
        return to_string(value);
    }
};

template <typename T>
struct get_str<T, true>
{
    static string convert(T value)
    {
        return to_string(static_cast<int>(value));
    }
};

template <typename T>
string get_str_rep(T value)
{
    return get_str<T, is_enum<T>::value>::convert(value);
}

template <>
string get_str_rep<string>(string value)
{
    return value;
}

template <>
string get_str_rep<const char *>(const char *value)
{
    return string(value);
}

template <>
string get_str_rep<char>(char c)
{
    string ans;
    ans.push_back(c);
    return ans;
}

// int main(int argc, char const *argv[])
// {
//     // std::cout << static_cast<int>(funcs::apple);
//     std::cout << get_str_rep("hi") << std::endl;
//     std::cout << get_str_rep(funcs::pear) << std::endl;
//     std::cout << get_str_rep(15514) << std::endl;
//     std::cout << get_str_rep('&') << std::endl;

//     return 0;
// }
