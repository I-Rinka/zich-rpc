#include <map>
#include <functional>
#include <sys/socket.h>
#include <arpa/inet.h>

// Initialize list?
class Response
{
};

// Initialize list?
class Request
{
};

class NetworkStuff
{
public:
    int _sockfd;
    void listen(const char *ipv4_address)
    {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in addr;
        // ....
        bind(_sockfd, (sockaddr *)&addr, sizeof(addr));

        // ???
    }

    char *read(int read_size)
    {
        // read(_sockfd, )
    }

    void write(char *buffer, int buffer_size, int write_size)
    {
    }
};

template <typename function_key>
class RPC_Core
{
    using Cb_func_type = std::function<void(RPC_Core *this, const Request *, Response *)>;

private:
    // should read web packet. Do the rpc call on the server, and then serialize response
    // to the client direction
    void event_handler();

    // MOVE the char* memory ownership to caller
    char *serialize_data(void *); // How should the parameters look like? Initialized_list?
                                  // Or something like arg**?
                                  // Ideal packet structure: [size(u32), key(cast from byte),arg:[...]]

protected:
    volatile std::map<function_key, Cb_func_type> func_map;

    void server_register_function(function_key key, Cb_func_type service_function)
    {
        // func_map[key] = service_function;
    }

    void server_register_function(function_key key, void(*service_function)(RPC_Core *, const Request *, Response *))
    {
        func_map[key] = service_function;
    }

    void server_run(); // address?

public:
    RPC_Core(/* args */); // For server side, it will register function before running.
                          // The register and server_run() function will be fired after
                          // registed all the function that client would call in the
                          // inherit server class
    ~RPC_Core();
    void client_request(function_key key, const Request *request, Response *response);
};

template <typename function_key>
inline void RPC_Core<function_key>::server_run()
{
    // create a new thread to hear incoming event from port
}

template <typename function_key>
inline void RPC_Core<function_key>::client_request(function_key key, const Request *request, Response *response)
{
    // Do some connection stuff. Serialize arguments in request and send it to network.
    // Then read data from network then parse the data to response.
    // Error handling includes time out
}

// The first version should be a very simple single socket request - response structure