// 定义客户端相关操作
#ifndef CLIENT
#define CLIENT

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
class CLIENT
{
public:
private:
    // 套接字部分
    int clt2rpc_fd; // 用于和rpc注册中心通信
    int clt2ser_fd; // 用于和服务端通信
    // 服务端的端口和编号
    sockaddr_in ser_addr;
    // json 部分
    json clt_j;
};

#endif