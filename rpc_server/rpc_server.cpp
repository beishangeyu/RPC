#include "rpc_server.h"

void RPC::rpc_init(string ip, short port)
{
    // 设定身份信息
    rpc_msg["identity"] = "RPC";
    // 初始化socket
    rpc_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (rpc_fd == -1)
    {
        cerr << "创建RPC注册中心套接字失败\n";
        return;
    }
    // 绑定socket
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(port);
    if (bind(rpc_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        // 关闭套接字并退出
        cerr << "绑定RPC注册中心套接字失败\n";
        close(rpc_fd);
        return;
    }
    // 开始监听
    listen(rpc_fd, SOMAXCONN);
}

void RPC::rpc_deal()
{
    // 接受来自 client 的连接
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int conc = accept(rpc_fd, (sockaddr *)&addr, &addr_len);
    char buffer[1024];
    // 从连接上读取数据
    json recv_msg;
    int byte = read(conc, buffer, sizeof(buffer) - 1);
    if (byte > 0)
    {
        buffer[byte] = '\0';
        recv_msg = recv_msg.parse(buffer);
    }
    if (recv_msg["identity"] == SERVER)
    {
        rpc_dealclient();
    }
    else
    {
        rpc_dealserver();
    }
}

void RPC::rpc_dealclient()
{
}

void RPC::rpc_dealserver()
{
}
