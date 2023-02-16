# Excuteble Socket Core

Use light weight lib `mpack` for serialization <https://github.com/ludocode/mpack>

ZilCh RPC.

## Todo

- [x] 字符串切割BUG
- [x] 超长字符串通信
- [x] 函数注册
- [ ] 析沟函数，同时禁止掉拷贝函数

## 设计

本函数库简单的封装了`boost::aiso`的socket通信的内容,以实现简单的双端通信。

同时为用户提供了简单的回调函数注册接口，以方便异步过程的调用（接受消息时）。

![](https://cdn.jsdelivr.net/gh/I-Rinka/picTure//20210220001617.png)

### 项目树

```shell
.
├── build
├── lib
│   └── socket_core.hpp
├── LICENSE
├── README.md
├── CMakeLists.txt
├── client_demo_main.cpp
└── server_demo_main.cpp
```

其中`lib/socket_core.hpp`即为本项目——一个对asio封装的头文件。

`client_demo_main.cpp`展示了客户端的代码，`server_demo_main.cpp`展示了服务器端的使用代码。

## 如何使用

### 使用方法

引入头文件：

```c++
#include "socket_core.hpp"
```

### 客户端

客户端运行时需要关注的有以下几点：

#### 实例化

客户端示例化时，需要指定连接的服务器的ip与服务器的对应端口：

```
Socket_Core Client("127.0.0.1", "1233");
```

#### 回调函数注册

通过以上步骤实例化后，客户端会自动创建一个线程，用于托管传入信息的异步调用，因此此时需要用户指定异步过程发生时的回调函数。

信息传入，即客户端收到来自服务器的字符串（一片不可修改的内存空间）`const char*`。通过注册回调函数，用户就可自定义对传入信息的`const char*`的操作。

注册回调函数有两种方法，第一种是通过`Client.Recive_Callback`直接指定：

```c++
Client.Recive_Callback = [](const char *message) 
{
    /*do some thing about the const chat *message */
    
 printf("来了一条消息!\n它的内容是:\n");
    std::cout<<message<<std::endl; 
};
```

第二种是通过方法`Server.RegistCallback(function(const char*))`注册回调函数：

```c++
auto ThisIsAFunction = [](const char *message) {
    
    /*do some thing about the const chat *message */
    
        printf("来了一条消息!\n它的内容是:\n");
        std::cout << message << std::endl;
    };

Server.RegistCallback(ThisIsAFunction);
```

**推荐使用后一种方法！**

上述注册函数的语法推荐使用C11的lamba表达式，这是一种方便快捷的注册匿名函数的方法：

![lambda 表达式的结构化元素](https://docs.microsoft.com/zh-cn/cpp/cpp/media/lambdaexpsyntax.png?view=msvc-160)

更多关于lambda表达式的说明详见：

<https://docs.microsoft.com/zh-cn/cpp/cpp/lambda-expressions-in-cpp?view=msvc-160>

[C++11 lambda表达式精讲 (biancheng.net)](http://c.biancheng.net/view/3741.html)

#### 方法

- Write(const char *msg)：向远程发送信息
- RegistCallback( std::function<void(const char *)> Recive_Callback)：注册回调函数
- 发送文件(const char *文件路径)：未实现

#### Tips

- 回调函数会接受远端传来的`const char*`，即这段内存是不允许修改的。若要对内容进行处理，建议使用`strnlen(message,Socket_Core::max_len)`查询收到的字符串的长度后再拷贝到指定位置`strncpy(dest,message,len)`后再做处理

### 服务器端

服务器端和客户端操作类似，不过实例化时只需要指定端口而无需指定服务器的ip地址（如：0.0.0.0）

有两种实例化方法：1. 端口号为`int` 或者 2.端口号为字符串

```c++
Socket_Core Server("1233");
```

```c++
Socket_Core Server(1233);
```

#### 方法

- Write(const char *msg)：向远程发送信息
- RegistCallback( std::function<void(const char *)> Recive_Callback)：注册回调函数
- 客户端连接时回调函数注册：未实现
- 发送文件(const char *文件路径)：未实现
