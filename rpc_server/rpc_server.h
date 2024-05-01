// 定义 rpc 注册中心类和相关操作
// 类似于服务器端
#ifndef RPC
#define RPC
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <map>
#include "../json/json.hpp"
using namespace std;
using json = nlohmann::json;

class RPC
{
public:
    void rpc_init();       // 初始化
    void rpc_listen();     // 开启监听
    void rpc_dealclient(); // 处理 client 连接, 向 client 发送服务器地址
    void rpc_dealserver(); // 处理 server 连接, 注册服务

private:
    // 监听套接字
    int socket_fd;
    // 记录服务器和对应的服务, 假设每个服务的名字都不同
    map<string, string> ip2func; // ip地址, 服务名
};

#endif