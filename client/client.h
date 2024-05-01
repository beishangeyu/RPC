// 定义客户端相关操作
#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <cstdlib>
#include <map>
#include <string>
#include "../json/json.hpp"
using namespace std;
using json = nlohmann::json;

// 服务器类
class CLIENT
{
public:
    void client_init(); // 初始化连接
    void client_pull(); // 从 rpc 获取服务器地址
    void client_call(); // 向服务端申请调用
private:
    int client_fd; // 客户端套接字
    json client_msg;
};

#endif