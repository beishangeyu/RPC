// 定义服务器端相关操作
#ifndef SERVE_H
#define SERVE_H

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

// 服务器类
class SERVE
{
public:
    // 初始化
    void serve_init();
    // 处理客户端
    void private :
        // 端口号和地址
        sockaddr_in address;
    // 监听套接字
    int serve_fd;
    // 已注册的函数列表, 每个服务端各自拥有一个
    map<string, int> func_list;
    // json 部分
    json ser_j;
};

#endif