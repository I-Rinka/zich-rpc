#include "socket_core.hpp"
int main(int argc, char const *argv[])
{
    Socket_Core Client("127.0.0.1", "1233");
    char *line = (char *)malloc(1000000);
    while (std::cin.getline(line, 100000))
    {
        //写是同步事件
        // std::cout << line << std::endl;
        Client.write(line);
    }
    return 0;
}
