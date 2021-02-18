#include "socket_core.hpp"
int main(int argc, char const *argv[])
{
    Socket_Core Server(1233);
    char line[200];
    while (std::cin.getline(line, 200))
    {
        //写是同步事件
        Server.write(line);
    }
    return 0;
}