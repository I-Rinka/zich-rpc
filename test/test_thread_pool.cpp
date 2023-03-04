#include "../src/server/thread_pool.h"

using namespace std;
int main(int argc, char const *argv[])
{
    ThreadPool tp(10);

    tp.AddTask([]
               { cout << "Hello world" << endl; this_thread::sleep_for(chrono::milliseconds(100)); });

    // future.get();

    tp.~ThreadPool();
    cout << "你好" << endl;
    return 0;
}
