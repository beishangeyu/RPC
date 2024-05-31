#include "client.h"
#include "client.cpp"

using namespace std;

int main(int argc, char *argv[])
{

    if (argc == 2 && string(argv[1]) == "-h")
    {
        // 输出帮助信息
        cout << "\t-l + 注册中心 ip 地址\n\t-p + 注册中心端口号\n";
        return 0;
    }

    string rpc_ip = string(argv[2]);
    short rpc_port = stoi(argv[4]);
    Client my_client;
    my_client.client_init(rpc_ip, rpc_port);
    my_client.client_start();
}