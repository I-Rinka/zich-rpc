#include "../src/server/thread_pool.h"

using namespace std;
int main(int argc, char const *argv[])
{
    ThreadPool tp(10);

    auto rs = tp.AddTask([]() -> int
                         {
                             cout << "Hello world" << endl;
                             //    this_thread::sleep_for(chrono::milliseconds(100));
                             throw runtime_error("test error"); return 2; });

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

    cout << "running threads:" << tp.GetWorkingThreadsNum() << endl;
    return 0;
}
