// 定义 rpc 注册中心类和相关操作
// 类似于服务器端
#ifndef RPC_H
#define RPC_H
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <pthread.h>
#include <map>
#include "../json/json.hpp"
using namespace std;
using json = nlohmann::json;
#define SOMAXCONN 15
#define SERVER "Server"
#define CLIENT "Client"

class RPC
{
public:
    void rpc_init(string ip, short port); // 初始化
    void rpc_dealclient();                // 处理 client 连接, 向 client 发送服务器地址
    void rpc_dealserver();                // 处理 server 连接, 注册或者删除服务
    void rpc_deal();

private:
    int rpc_fd;                  // rpc 套接字
    map<string, string> func2ip; // 服务表, <服务名称, ip地址>, 记录每个服务在哪个服务端那里 (假设每个服务端拥有的服务都不相同)
    pthread_mutex_t lock;        // 互斥锁保护服务表
    json rpc_msg;
};

#endif