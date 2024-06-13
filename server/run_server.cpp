#include "server.h"
#include "server.cpp"
using namespace std;

int main(int argc, char *argv[])
{
    Server my_server;
    string ip = ""; // 默认为空
    short port;
    int func_num;

    if (argc == 2 && string(argv[1]) == "-h")
    {
        // 输出帮助信息
        cout << "\t-l + 服务器监听的 ip 地址, 默认监听所有ip地址\n\t-p + 注册中心端口号\n\t-n + 支持的服务个数\n";
        return 0;
    }
    else if (argc > 2)
    {
        // 有指定ip
        if (string(argv[1]) == "-l")
        {
            ip = string(argv[2]);
            port = stoi(argv[4]);
            func_num = stoi(argv[6]);
        }
        // 没指定ip, 只设置端口
        else
        {
            port = stoi(argv[2]);
            func_num = stoi(argv[4]);
        }
    }
    my_server.server_init(ip, port, func_num, "127.0.0.1", 8000);
    my_server.server_start();
}