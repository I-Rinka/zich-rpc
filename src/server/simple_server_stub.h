#include "../packet/function_traits.h"
#include "../packet/serializer.h"
#include "../packet/packet_parser.h"
#include "../packet/get_str_rep.h"
#include "../packet/function_call_tule.h"
#include "../network/socket_connect.h"
#include "../network/length_prefixed_socket.h"

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <thread>

class SDecoder : public Decoder
{
private:
    std::string _current_str;
    std::vector<Element> _current_elements;
    size_t _para_number;

public:
    SDecoder() = default;
    SDecoder(std::string &&packet)
    {
        this->AddString(std::forward<std::string>(packet));
    };

    //! \brief Call it at first when network comes packet
    void AddString(std::string &&packet)
    {
        _current_str = std::move(packet);
        _current_elements = ParsePack(_current_str);
        std::reverse(_current_elements.begin(), _current_elements.end());
        _para_number = _current_elements.size();
    }

    virtual bool ReachEnd() override
    {
        return _current_elements.empty();
    }

    virtual bool ElementNumberEqual(size_t para_number) override
    {
        return para_number == _para_number;
    }

    virtual bool DecodeNextBool() override
    {
        Element t = _current_elements.back();
        _current_elements.pop_back();

        if (t.type != ElementType::INT)
        {
            throw std::runtime_error("Decode Bool Error");
        }

        return (bool)t.int_val;
    }

    virtual double DecodeNextDouble() override
    {
        Element t = _current_elements.back();
        _current_elements.pop_back();

        if (t.type != ElementType::FLOAT)
        {
            throw std::runtime_error("Decode f64 Error");
        }

        return t.float_val;
    }

    virtual long long DecodeNextInt() override
    {
        Element t = _current_elements.back();
        _current_elements.pop_back();

        if (t.type != ElementType::INT)
        {
            throw std::runtime_error("Decode i64 Error");
        }

        return t.int_val;
    }

    virtual std::string DecodeNextString() override
    {
        Element t = _current_elements.back();
        _current_elements.pop_back();

        if (t.type != ElementType::INT)
        {
            throw std::runtime_error("Decode i64 Error");
        }

        return t.data_str;
    }
};

class SServerStub
{
private:
    ServerSocket<LengthPrefixedSocket> *_ss;
    std::map<std::string, std::function<void(LengthPrefixedSocket &, SDecoder &)>> _func_map;

public:
    void ProcessSocket(LengthPrefixedSocket &client_socket)
    {
        try
        {
            SDecoder decoder(client_socket.recv());
            try
            {
                string func_key = decoder.DecodeNextString();
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

    SServerStub(uint16_t port) : _ss(new ServerSocket<LengthPrefixedSocket>(port))
    {
        ServerThread();
    };

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
        _func_map[function_name] = [](LengthPrefixedSocket &client_socket, SDecoder &decoder)
        {
            auto tuple = DecodeParameters(decoder, callable);

            // call tuple
            typename function_traits<Function>::return_type ans = CallTuple(callable, tuple);

            // serialize ans

            // send serialized ans
            client_socket.send("hello");
        };
    }
};