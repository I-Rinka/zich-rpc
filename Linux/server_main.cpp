#include "socket_core.hpp"
int main(int argc, char const *argv[])
{
    Socket_Core Server(1233);
    char *line=(char*)malloc(INT32_MAX);
    while (std::cin.getline(line, INT32_MAX))
    {
        //写是同步事件
        Server.write(line);
    }
    return 0;
}