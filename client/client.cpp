#include "client.h"

void CLIENT::client_init()
{
    // 生成随机数为 client 编号
    srand(static_cast<unsigned int>(time(nullptr)));
    string rand_number = to_string(rand());
    // 初始化套接字
    client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    client_msg["identity"] = "Client_" + rand_number;
}

void CLIENT::client_pull(string rps_ip, string port)
{
}

void CLIENT::client_call()
{
}
