#include <iostream>
#include <vector>
#include <memory>

template <typename T>
class my_unique : public std::unique_ptr<T>
{
public:
    T* p;
    my_unique(T *pointer) : std::unique_ptr<T>(pointer) {
        p = pointer;
    }
    ~my_unique()
    {
        std::cout << "destruct" << std::endl;
        // free(p); It will complain a double free situation. which means unique_ptr works
    }
};

int main(int argc, char const *argv[])
{
    int *p;
    int a = 0;
    std::cout << sizeof(p) << std::endl;
    my_unique<int> p2(new int(2));
    std::cout << sizeof(p2) << std::endl;
    auto v1 = std::vector<int>();
    std::cout << sizeof(v1) << std::endl;
    auto v = std::vector<long long>();
    std::cout << sizeof(v) << std::endl;

    return 0;
}
