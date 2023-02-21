#include "zcrpc.h"
#include <functional>

enum class function_name
{
    hello,
    world
};

class ServerRPC : public RPC_Core<function_name>
{
public:
    void Hi(const Request *rq, Response *rs);
    ServerRPC()
    {
        RPC_Core::server_register_function(function_name::hello, &this->Hi);
    }
};

class ClientRPC : public RPC_Core<function_name>
{
};