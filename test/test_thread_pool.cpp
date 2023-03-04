#include "../src/server/thread_pool.h"

class Myclass
{
public:
    std::string str;
    Myclass(std::string &&str)
    {
        this->str = std::move(str);
    }

    Myclass(Myclass &&other)
    {
        str = std::move(other.str);
    };

    Myclass &operator=(Myclass &&other)
    {
        str = std::move(other.str);
        return *this;
    };
};

int func(Myclass cls)
{
    std::cout << cls.str << endl;
    return cls.str.size();
}

using namespace std;
int main(int argc, char const *argv[])
{
    ThreadPool tp(10);

    auto rs = tp.AddTask([]() -> int
                         {
                             cout << "Hello world" << endl;
                             //    this_thread::sleep_for(chrono::milliseconds(100));
                             throw runtime_error("test error"); return 2; });

    Myclass tmp(string("Hello world"));

    // auto func = [](Myclass cls) -> int
    // {
    //     std::cout << cls.str << endl;
    //     return cls.str.size();
    // };

    auto f = std::bind(func, std::move(tmp));
    f();

    auto rs2 = tp.AddTask([](Myclass cls) -> int
                          {
        std::cout << cls.str << endl;
        return cls.str.size(); },
                          Myclass(std::move(tmp)));

    this_thread::sleep_for(chrono::milliseconds(1000));
    try
    {
        cout << rs.get() << endl;
    }
    catch (const std::exception &e)
    {
        // error will be catch by the .get(). Otherwise it never get feedback
        std::cerr << e.what() << endl;
    }

    cout << rs2.get() << endl;

    cout << "running threads:" << tp.GetWorkingThreadsNum() << endl;
    return 0;
}
