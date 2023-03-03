# ZiCh RPC.

ZilCh RPC. A highly scalable concise RPC lib

Serialization design: tuple -> pack.

Format design:

```txt
    Pack: `size:`<usigned-int>`\n`Element
    Element -> i64 | f64 | string | array | '\n' | Element
    i64 -> `i64:`<long-long>
    f64 -> `f64:`<double>
    string -> `string:`<no-zero-c-str>
    array -> `array:[`Element`\n]`
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

## Todo

- [ ] 函数指针绑定支持
- [ ] 线程池化
- [ ] 增加server端错误处理
- [ ] Linux 单独设置epoll编译选项
- [ ] Clear / Close 选项，当调用失败时