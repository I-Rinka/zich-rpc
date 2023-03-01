#include <iostream>
#include <type_traits>
using namespace std;

// enum class funcs
// {
//     apple,
//     pear,
//     orange
// };
#ifndef __Get_Str_Rep_H__
#define __Get_Str_Rep_H__
template <typename T, bool is_enum>
struct __get_str
{
    static string convert(T value)
    {
        return to_string(value);
    }
};

template <typename T>
struct __get_str<T, true>
{
    static string convert(T value)
    {
        return to_string(static_cast<int>(value));
    }
};

template <typename T>
string GetStrRep(T value)
{
    return __get_str<T, is_enum<T>::value>::convert(value);
}

template <>
string GetStrRep<string>(string value)
{
    return value;
}

template <>
string GetStrRep<const char *>(const char *value)
{
    return string(value);
}

template <>
string GetStrRep<char>(char c)
{
    string ans;
    ans.push_back(c);
    return ans;
}
#endif