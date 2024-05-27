#include "client.h"
// #include "client.cpp"
using namespace std;

int main(int argc, char *argv[])
{
    if (argc == 2 && argv[1] == "-h")
    {
        // TODO: 输出帮助信息, 服务列表
        return;
    }
    string rpc_ip = argv[2];
    short rpc_port = stoi(argv[4]);
    Client my_client;
    my_client.client_init(rpc_ip, rpc_port);
    my_client.client_start();
}