#include "server.h"

// 参数: ip地址, 端口号, 要开启的服务数量, 服务列表
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
}

void Server::server_start()
{
    // 传入函数名, 注册本地服务表
    // 向注册中心注册服务
}

// 向注册中心注册服务
void Server::registe()
{
}

// 向本地服务表中添加
void Server::add_func_map(int func_idx)
{
    // 服务器支持的服务一共有12种
    switch (func_idx)
    {
    case 0:
        /* code */
        break;
    case 1:
        /* code */
        break;
    case 2:
        /* code */
        break;
    case 3:
        /* code */
        break;
    case 4:
        /* code */
        break;
    case 5:
        /* code */
        break;
    case 6:
        /* code */
        break;
    case 7:
        /* code */
        break;
    case 8:
        /* code */
        break;
    case 9:
        /* code */
        break;
    case 10:
        /* code */
        break;
    case 11:
        /* code */
        break;
    default:
        break;
    }
}
