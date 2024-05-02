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
#include "../json/json.h"
using namespace std;
using json = nlohmann::json;
#define MAXTHREAD 10

class RPC
{
public:
    void rpc_init(string ip, short port); // 初始化
    void rpc_start();                     // 开启服务

private:
    int rpc_fd;                  // rpc 套接字
    map<string, int> func2port;  // 服务表, <服务名称, port>
    map<string, string> func2ip; // 服务表, <服务名称, ip地址>, 记录每个服务在哪个服务端那里 (假设每个服务端拥有的服务都不相同)
    pthread_mutex_t lock;        // 互斥锁保护服务表
    json rpc_msg;

private:
    static void *worker(void *arg); // 线程的工作函数
    json rpc_dealclient(json);      // 处理 client 连接, 向 client 发送服务器地址
    json rpc_dealserver(json);      // 处理 server 连接, 注册或者删除服务
    void rpc_deal();                // 线程工作的实际逻辑
};

#endif