#include <iostream>
#include <sstream>
#include <string>
#include <vector>

enum class ElementType
{
    INT,
    FLOAT,
    STRING,
    ARRAY
};

struct Element
{
    ElementType type;
    std::string data_str;                // store the raw string representation of the data
    std::vector<Element> array_elements; // only used if type == ARRAY
    long long int_val;                   // only used if type == INT
    double float_val;                    // only used if type == FLOAT
};

std::pair<std::string, int> parse_string(const std::string &str, int start)
{
    std::string result;
    while (start < str.size() && str[start] != ':')
    {
        ++start;
    }
    if (start == str.size())
    {
        throw std::invalid_argument("Invalid string format");
    }
    ++start; // Move past the colon

    while (start < str.size() && str[start] != '\n')
    {
        if (start >= str.size())
        {
            throw std::invalid_argument("Invalid string format");
        }
        if (str[start] == '\\')
        {
            // If the next character is a backslash, check if it's an escape sequence
            if (start + 1 >= str.size())
            {
                throw std::invalid_argument("Invalid string format");
            }
            ++start; // Move past the backslash
            if (str[start] == 'n')
            {
                result += '\n'; // Replace '\n' with an actual newline character
            }
            else
            {
                result += str[start]; // Keep the original character
            }
        }
        else
        {
            result += str[start]; // Add the character to the result string
        }
        start++;
    }

    return {result, start};
}

// Parse an element of the syntax, starting at the specified position in the input string.
// Returns the parsed element and the position of the next character after the element.
std::pair<Element, std::size_t> parse_element(const std::string &input, std::size_t pos)
{
    Element elem;
    elem.data_str = "";

    if (input[pos] == 'i' && input.substr(pos, 4) == "i64:")
    {
        elem.type = ElementType::INT;
        elem.data_str = input.substr(pos, input.find('\n', pos) - pos);
        elem.int_val = std::stoll(elem.data_str.substr(4));
        pos += elem.data_str.size();
    }
    else if (input[pos] == 'f' && input.substr(pos, 4) == "f64:")
    {
        elem.type = ElementType::FLOAT;
        elem.data_str = input.substr(pos, input.find('\n', pos) - pos);
        elem.float_val = std::stod(elem.data_str.substr(4));
        pos += elem.data_str.size();
    }
    else if (input[pos] == 's' && input.substr(pos, 7) == "string:")
    {
        elem.type = ElementType::STRING;
        auto ans = parse_string(input, pos);

        elem.data_str = ans.first;
        pos = ans.second;
    }
    else if (input[pos] == 'a' && input.substr(pos, 7) == "array:[")
    {
        elem.type = ElementType::ARRAY;
        elem.data_str = input.substr(pos, input.find(']', pos) - pos + 1);
        pos = pos + 7; // skip "array:[" prefix
        while (input[pos] != ']')
        {
            auto sub_elem = parse_element(input, pos);
            elem.array_elements.push_back(sub_elem.first);
            pos = sub_elem.second;
        }
        ++pos; // skip over closing ']'
    }
    else if (input[pos] == '\n')
    {
        elem.type = ElementType::STRING;
        elem.data_str = "\n"; // newline character
    }

    ++pos; // skip over newline character
    return {elem, pos};
}

// Parse the entire syntax, starting at the beginning of the input string.
// Returns a vector of parsed elements.
std::vector<Element> parse_pack(const std::string &input)
{
    std::vector<Element> pack_elements;
    std::size_t pos = 0;
    std::string size_str;
    std::getline(std::istringstream(input), size_str); // read size from input string
    std::size_t pack_size = std::stoul(size_str.substr(5));
    pos += size_str.size() + 1;

    while (pos < input.size() && pack_elements.size() < pack_size)
    {
        auto elem = parse_element(input, pos);
        pack_elements.push_back(elem.first);
        pos = elem.second;
    }

    return pack_elements;
}

std::ostream &operator<<(std::ostream &os, const Element &elem)
{
    os << "type: ";
    switch (elem.type)
    {
    case ElementType::INT:
        os << "int, value: " << elem.int_val;
        break;
    case ElementType::FLOAT:
        os << "float, value: " << elem.float_val;
        break;
    case ElementType::STRING:
        os << "string, value: " << elem.data_str;
        break;
    case ElementType::ARRAY:
        os << "array, size: " << elem.array_elements.size();
        os << "array: ";
        for (auto i : elem.array_elements)
        {
            os << i;
            os << "------\n";
        }
        // os << "]";
        break;
    }
    os << "\n";
    return os;
}
// int main(int argc, char const *argv[])
// {
//     auto E = parse_pack("size:5\ni64:123123343\nstring:Hello\\nworld!!\nstring: \nstring:\\n\\nnihao\narray:[i64:123123343\nstring:Hello\\nworld!!\nstring: \nstring:\\n\\nnihao\n]");
//     // auto E = parse_pack("size:2\ni64:15556\nf64:3.14\n");
//     for (auto i : E)
//     {
//         std::cout << i;
//     }

//     return 0;
// }