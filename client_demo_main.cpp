#include "socket_core.hpp"
int main(int argc, char const *argv[])
{
    Socket_Core Client("127.0.0.1", "1233");
    char *line = (char *)malloc(1000);
    Client.Recive_Callback = [](const char *message) {  printf("来了一条消息!\n它的内容是:\n");std::cout<<message<<std::endl; };
    while (std::cin.getline(line, 1000))
    {
        //写是同步事件
        Client.Write(line);
    }
    return 0;
}
