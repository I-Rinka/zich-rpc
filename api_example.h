#include "zcrpc.h"

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
        // bind and placeholders
        RPC_Core::server_register_function(function_name::hello, Hi);
    }
};

class ClientRPC : public RPC_Core<function_name>
{
};