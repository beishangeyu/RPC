// 定义客户端相关操作
#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <map>
#include <mutex>
#include <thread>
#include <regex>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include "../json/json.hpp"
#include "../json/json.h"
using namespace std;
using json = nlohmann::json;

// 服务器类
class Client
{
public:
    void client_init(string rpc_ip, short rpc_port); // 初始化连接
    void client_start();                             // 请求服务
private:
    void set_timeout(int fd, int sec);
    int client_pull(string func); // 从 rpc 获取服务器地址
    int client_call(string func); // 向服务端申请调用
    int with_rpc;                 // 和 rpc 通信的套接字
    int with_ser;                 // 和server通信的套接字
    string rpc_ip;
    short rpc_port;
    string server_ip;
    short server_port;
    bool is_stop = false;
};

#endif