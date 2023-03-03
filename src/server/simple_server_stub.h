#include "../util/function_traits.h"
#include "../packet/serializer.h"
#include "../network/socket_connect.h"
#include "../network/length_prefixed_socket.h"
#include "../packet/netpacket_object.h"
#include "../packet_protocal/simple_protocal.h"
#include "function_call_tule.h"

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <thread>

#ifndef __SIMPLE_SERVER_STUB_H__
#define __SIMPLE_SERVER_STUB_H__

template <size_t arity>
struct __func_args_decode_helper
{
    template <typename F>
    static auto call(F callable, SDecoder &decoder) -> typename function_traits<F>::return_type
    {
        auto args_tuple = DecodePacket<typename function_traits<F>::args_tuple>(decoder);
        return CallTuple(callable, args_tuple);
    }
};

// no args
template <>
struct __func_args_decode_helper<0>
{
    template <typename F>
    static auto call(F callable, SDecoder &decoder) -> typename function_traits<F>::return_type
    {
        return callable();
    }
};

template <typename return_type>
struct __func_return_helper
{
    template <typename F>
    static void call(F callable, SDecoder &decoder, TCPSocket &socket)
    {
        auto ans = __func_args_decode_helper<function_traits<F>::arity>::call(callable, decoder);
        SEncoder encoder;
        std::string serialized_ans = EncodePacket(encoder, ans);
        socket.send(serialized_ans);
    }
};

// no return value
template <>
struct __func_return_helper<void>
{
    template <typename F>
    static void call(F callable, SDecoder &decoder, TCPSocket &socket)
    {
        __func_args_decode_helper<function_traits<F>::arity>::call(callable, decoder);
    }
};

class SServerStub
{
private:
    ServerSocket<LengthPrefixedSocket> *_ss;
    using func_map_t = std::map<std::string, std::function<void(LengthPrefixedSocket &, SDecoder &)>>;
    func_map_t _func_map;

    void ProcessSocket(LengthPrefixedSocket &client_socket)
    {
        try
        {
            std::string recv_str = client_socket.recv();
            SDecoder decoder(std::move(recv_str));
            try
            {
                std::string func_key = decoder.DecodeNextString();
                if (_func_map.find(func_key) == _func_map.end())
                {
                    std::cout << "No such function!" << std::endl;
                    return;
                }

                try
                {
                    _func_map[func_key](client_socket, decoder);
                }
                catch (const std::exception &e)
                {
                    std::cout << "Call function failed" << std::endl;
                }
            }
            catch (const std::exception &e)
            {
                std::cout << "No function key!" << std::endl;
                return;
            }

            ProcessSocket(client_socket); // Infinetly process client
        }
        catch (const std::exception &e)
        {
            std::cout << "connection lost" << std::endl;
            return;
        }
    }

    void ServerThread()
    {
        auto client = _ss->accept();

        ProcessSocket(client);

        this->ServerThread();
    }

public:
    SServerStub(uint16_t port) : _ss(new ServerSocket<LengthPrefixedSocket>(port)){};

    ~SServerStub()
    {
        if (_ss)
        {
            delete _ss;
        }
    };

    template <typename Key, typename Function>
    void bind(Key function_name, Function callable)
    {
        std::string key = GetStrRep(function_name);
        _func_map[key] = [=](LengthPrefixedSocket &client_socket, SDecoder &decoder)
        {
            __func_return_helper<typename function_traits<Function>::return_type>::call(callable, decoder, client_socket);
        };
    }

    void start()
    {
        ServerThread();
    }
};
#endif