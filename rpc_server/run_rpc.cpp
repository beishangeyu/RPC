#include "rpc_server.h"
#include "rpc_server.cpp"

int main()
{
    Rpc_server my_rpc;

    string ip = "127.0.0.1";
    short port = 8000;
    my_rpc.rpc_init(ip, port);
    my_rpc.rpc_start();
}