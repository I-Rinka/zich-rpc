#include "../src/server/thread_pool.h"

using namespace std;
int main(int argc, char const *argv[])
{
    ThreadPool tp(10);

    tp.AddTask([]
               { cout << "Hello world" << endl; 
            //    this_thread::sleep_for(chrono::milliseconds(100)); 
               });

    // future.get();

    cout << "你好" << endl;
    cout << "你好2" << endl;
    cout << "你好3" << endl;
    cout << "你好4" << endl;
    // this_thread::sleep_for(chrono::milliseconds(100));
    cout << "running threads:" << tp.GetWorkingThreadsNum() << endl;
    return 0;
}
