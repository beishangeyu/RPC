// 定义服务器端相关操作
#ifndef SERVE_H
#define SERVE_H

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
#include "../json/json.hpp"
#include "../json/json.h"
using namespace std;
using json = nlohmann::json;
#define MAXFUNCNUM 12;
#define MAXTHREAD 10

// 服务器类
class Server
{
public:
    void server_init(string, short, int, int[], string, short, int = 0); // 初始化
    void server_start();                                                 // 服务器开始运行
    ~Server();

private:
    void add_func_map(int);     // 向本地服务表中添加
    void deal_client();         // 处理客户端请求
    void set_timeout(int, int); // 设置连接超时时间

private:
    string server_ip;          // 服务端ip
    bool is_stop = false;      // 控制服务器关闭
    short server_port;         // 服务端端口号
    int with_clt_fd;           // 接受客户端连接套接字
    int with_rpc_fd;           // 连接注册中心套接字
    map<string, int> func2idx; // 本地服务表, 服务器端已有服务和对应编号
    int num_func;              // 服务端支持的服务的个数
private:
    // 这部分是服务器支持的函数
    double add(double a, double b) { return a + b; };
    double subtract(double a, double b) { return a - b; };
    double multiply(double a, double b) { return a * b; };
    double divide(double a, double b) { return a / b; };
    string concat(string a, string b) { return a + b; };
    string compare(double a, double b) { return (a > b ? "a > b" : "a <= b"); };
    string func1() { return "call func1\n"; };
    string func2() { return "call func2\n"; };
    string func3() { return "call func3\n"; };
    string func4() { return "call func4\n"; };
    string func5() { return "call func5\n"; };
    string func6() { return "call func6\n"; };
};

#endif