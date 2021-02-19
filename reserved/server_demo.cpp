#include "socket_core.hpp"
int main(int argc, char const *argv[])
{
    Socket_Core Server(1233);
    char *line = (char *)malloc(100000);
    while (1)
    {
        char c = '7';
        int i = 0;
        while (1)
        {
            if (i >= 4096)
            {
                std::cout << i << std::endl;
            }
            if (i == 100007)
            {
                *(line + i) = '8';
                break;
            }

            if (c == '\n')
            {
                break;
            }
            *(line + i) = c;
            i++;
        }
        Server.write(line);
    }
    return 0;
}