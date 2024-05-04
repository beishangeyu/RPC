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
class Server
{
public:
    void server_init(string, short, int, int[]); // 初始化
    void server_start();                         // 服务器开始运行
    void registe();                              // 编写注册的 json
    void add_func_map(int);                      // 向本地服务表中添加
private:
    string server_ip;          // 服务端ip
    short server_port;         // 服务端端口号
    int server_fd;             // 监听套接字
    map<string, int> func2idx; // 本地服务表, 服务器端已有服务和对应编号
    int num_func;              // 服务端支持的服务的个数
private:
    // 这部分是服务器支持的服务, 注册是可选的
    // 通过传入数组 或者 参数来选择
};

#endif