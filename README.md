# ZiCh RPC.

ZiCh RPC. A highly scalable concise RPC lib.

## Installation

Currently, you should compile the source code directly into you project. Which means no installation, copy file to project directory will just work.

```bash
mv ./src <base-directory-that-uses-this-lib>/zichrpc
```

```c++
#include "./zichrpc/zich_rpc.h"
// ... your code
```

## Example

Server:

```c++

ServerStub ss(PORT); // declare a server using PORT

ss.bind(RPCs::print_string, [](string str)  { print_string(str); }); // register a server function

ss.start(); // starts server

```

Cilent:

```c++

ClientStub cs; // declare a client caller

cs.connect("127.0.0.1", PORT); // connect to target server
cs.call(RPCs::print_string, "I am client"); // call the print_string function register before

cout << cs.call(RPCs::divide, 10, 3.14).as<double>() << endl; // call a function that returns double.

```

### RPC for any object

Now Zich RPC only support serializing basic type. However, since it defines std::string as the serialized packet type, users can easily create self-defined object serializer by using std::string.

Using self defined object is to specialize template `__encoder` and `__decoder`. Then following interface like NetPacket object works just fine.

For more details see `example/object_serializer_example.cpp`

## Details

This project is made up of:

- Full C++11 implementation. Therefore, it remade lots of wheel of the feature added by C++14 or C++17
- No any extra dependencies
- Wrapped socket implementation
- Function traits metaprogramming implementation
- Modern C++ threaded pool implementation
- Epoll implementation for Linux
- ...

### Serialization protocol 

This library supports theoretically supports XML, JSON, Msgpack, and any other kind of protocals that serialize basic type into network string.

Currently it uses a self-defined protocal that has following format:

Format design:

```txt
    Pack: `size:`<usigned-int>`\n`Element
    Element -> i64 | f64 | string | array | '\n' | Element
    i64 -> `i64:`<long-long>
    f64 -> `f64:`<double>
    string -> `string:`<no-zero-c-str>
    array -> `array:[`Element`\n]` // deprecated
```

By supporting f64 and i64, it supports every basic format that modern computer may have. 

By supporting string, it can send netflow directly or nested parse incoming packet string to support more complex type serialization.

### Serialization design

The serialization support composed type defined by `std::tuple` , or a single object. This library uses metaprogramming to recognise whether the user passes a composed tuple, or a single type.

std::tuple / Single object -> std::string


## Todo

- [X] 函数指针绑定支持
- [X] 线程池化
- [X] 增加server端错误处理
- [ ] Linux 单独设置epoll编译选项
- [X] Clear / Close 选项，当调用失败时
- [X] 可拓展性选项，允许用户自定义字符串解析为对象的函数
