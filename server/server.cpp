#include "server.h"

void Server::server_init(string ip, short port, int num, int func[])
{
    // 设置服务器基本参数
    num_func = num;
    server_ip = ip;
    server_port = port;
    // 创建套接字
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == -1)
    {
        cerr << "创建套接字失败\n";
        return;
    }
    // 允许地址重用
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        cerr << "设置地址重用失败" << endl;
        close(server_fd);
        return;
    }
    // 绑定地址到套接字
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr); // 将字符串转为网络字节序
    server_addr.sin_port = htons(port);
    if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        cerr << "绑定套接字失败\n";
        close(server_fd);
        return;
    }
    // 向本地服务表注册
    for (int i = 0; i < num; i++)
    {
        add_func_map(func[i]);
    }
    // 向注册中心注册
    registe();
}

void Server::server_start()
{
    // 传入函数名, 注册本地服务表
    // 向注册中心注册服务
}

// 向注册中心注册服务
void Server::registe()
{
    // 本地拥有的服务
    string func;
    for (auto &it : func2idx)
    {
        func += it.first + " ";
    }
}

// 向本地服务表注册函数
void Server::add_func_map(int func_idx)
{
    // 服务器支持的服务一共有12种
    switch (func_idx)
    {
    case 0:
        func2idx["add"] = 0;
        break;
    case 1:
        func2idx["subtract"] = 1;
        break;
    case 2:
        func2idx["multiply"] = 2;
        break;
    case 3:
        func2idx["divide"] = 3;
        break;
    case 4:
        func2idx["concat"] = 4;
        break;
    case 5:
        func2idx["compare"] = 5;
        break;
    case 6:
        func2idx["func1"] = 6;
        break;
    case 7:
        func2idx["func2"] = 7;
        break;
    case 8:
        func2idx["func3"] = 8;
        break;
    case 9:
        func2idx["func4"] = 9;
        break;
    case 10:
        func2idx["func5"] = 10;
        break;
    case 11:
        func2idx["func6"] = 11;
        break;
    default:
        break;
    }
}
