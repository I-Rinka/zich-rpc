#include "socket_core.hpp"
int main(int argc, char const *argv[])
{
    Socket_Core Server("1233");
    char *line = (char *)malloc(1000);

    //也可以这么写
    auto ThisIsAFunction = [](const char *message) {
        printf("来了一条消息!\n它的内容是:\n");
        std::cout << message << std::endl;
    };

    Server.RegistCallback(ThisIsAFunction);
    while (std::cin.getline(line, 1000))
    {
        Server.Write(line);
    }
    return 0;
}
