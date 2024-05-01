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
typedef void (*func_ptr)();

// 服务器类
class SERVE
{
public:
    void server_init();   // 初始化
    void server_listen(); // 开启监听
    void server_oprate(); // 向注册中心注册或者删除服务
private:
    sockaddr_in address;       // 端口号和地址
    int serve_fd;              // 监听套接字
    map<string, int> func2idx; // 服务器端已有服务和对应编号
    vector<func_ptr> func;     // 存储已有函数的函数指针
    json server_msg;
};

#endif