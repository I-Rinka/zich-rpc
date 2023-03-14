#include "../util/function_traits.h"
#include "../packet/serializer.h"
#include "../network/socket_connect.h"
#include "../network/length_prefixed_socket.h"
#include "../packet/netpacket_object.h"
#include "../packet_protocal/simple_protocal.h"
#include "thread_pool.h"
#include "function_call_tule.h"
#include <cassert>

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <thread>

#ifndef __THREADED_SERVER_STUB_H__
#define __THREADED_SERVER_STUB_H__

constexpr int THREADS_NUMBER = 10;

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

class TServerStub
{
private:
    ServerSocket<LengthPrefixedSocket> *_ss;
    using func_map_t = std::map<std::string, std::function<void(LengthPrefixedSocket &, SDecoder &)>>;
    func_map_t _func_map;

    ThreadPool _thread_pool;

    void ProcessSocket(LengthPrefixedSocket &client_socket)
    {
        if (this->_status == stub_status::stop)
        {
            return;
        }

        try
        {
            std::string recv_str = client_socket.recv();
            SDecoder decoder(std::move(recv_str));
            try
            {
                std::string func_key = decoder.DecodeNextString();
                if (_func_map.find(func_key) == _func_map.end())
                {
                    std::cerr << "Did not bind such a function! Target:" << client_socket.IP << ':' << client_socket.port << std::endl;
                    return;
                }

                try
                {
                    _func_map[func_key](client_socket, decoder);
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Call function failed! Target:" << client_socket.IP << ':' << client_socket.port << std::endl;
                    return;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "No function key! Target:" << client_socket.IP << ':' << client_socket.port << std::endl;
                return;
            }

            ProcessSocket(client_socket); // Infinetly process client
        }
        catch (const std::exception &e)
        {
            std::cerr << "Connection lost. Target:" << client_socket.IP << ':' << client_socket.port << std::endl;
            return;
        }
    }

    struct socket_functor
    {
        LengthPrefixedSocket _socket;
        TServerStub *_ss_ptr;
        socket_functor(LengthPrefixedSocket &&socket, TServerStub *_this) : _socket(std::move(socket)), _ss_ptr(_this) {}

        void operator()()
        {
            _ss_ptr->ProcessSocket(_socket);
            _socket.close();
        }
    };

    enum class stub_status
    {
        stop,
        running
    };

    volatile stub_status _status = stub_status::stop;

    void ServerThread()
    {
        while (auto client = _ss->accept())
        {
            if (_status == stub_status::stop)
            {
                client.close();
            }
            else
            {
                // Movable functor
                _thread_pool.AddTask(socket_functor(std::move(client), this));
            }
        }
    }
    uint16_t _port;

public:
    TServerStub(uint16_t port) : _ss(new ServerSocket<LengthPrefixedSocket>), _thread_pool(THREADS_NUMBER), _port(port){};

    ~TServerStub()
    {
        _status = stub_status::stop;

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
        _status = stub_status::running;

        _ss->bind(_port);
        _ss->listen();
        ServerThread();
    }

    void stop()
    {
        _status = stub_status::stop;
    }

    void reStart()
    {
        _status = stub_status::running;
    }
};
#endif