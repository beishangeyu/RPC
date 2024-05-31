#include "rpc_server.h"
#include "rpc_server.cpp"

int main(int argc, char *argv[])
{
    // 默认监听ip地址和端口号
    Rpc_server my_rpc;
    string ip = "127.0.0.1";
    short port = 8000;
    if (argc == 2 && string(argv[1]) == "-h")
    {
        // 输出帮助信息
        cout << "\t-l + 注册中心 ip 地址\n\t-p + 注册中心端口号\n";
        return 0;
    }
    if (argc == 5)
    {
        ip = string(argv[2]);
        port = stoi(argv[4]);
    }
    else if (argc == 3)
        my_rpc.rpc_init(ip, port);
    my_rpc.rpc_start();
}