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
#include <map>
#include <mutex>
#include <thread>
#include <algorithm>
#include <string>
#include "../json/json.hpp"
#include "../json/json.h"
using namespace std;
using json = nlohmann::json;
#define MAXTHREAD 10

class Rpc_server
{
public:
    Rpc_server(){};
    void rpc_init(string ip, short port); // 初始化
    void rpc_start();                     // 开启服务
    ~Rpc_server();

private:
    int rpc_fd;                  // rpc 套接字
    bool is_stop = false;        // 是否关闭注册中心
    map<string, int> func2port;  // 服务表, <服务名称, port>
    map<string, string> func2ip; // 服务表, <服务名称, ip地址>, 记录每个服务在哪个服务端那里 (假设每个服务端拥有的服务都不相同)
    mutex lock;

private:
    json rpc_dealclient(json);         // 处理 client 连接, 向 client 发送服务器地址
    json rpc_dealserver(json);         // 处理 server 连接, 注册或者删除服务
    void rpc_deal();                   // 线程工作的实际逻辑
    void set_timeout(int fd, int sec); // 为连接设置超时时间
};

#endif