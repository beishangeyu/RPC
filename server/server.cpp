#include "server.h"
void Server::server_init(string ip, short port, int num, int func[], int ipv6,
    string rpc_ip, short rpc_port)
{
    // 设置服务器基本参数
    num_func = num;
    server_ip = ip;
    server_port = port;
    // 创建并绑定ipv6套接字
    if (ipv6)
    {
        server_fd = socket(AF_INET6, SOCK_STREAM, 0);
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
        // 初始化服务器地址
        sockaddr_in6 server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin6_family = AF_INET6;
        // 监听特定的网络地址
        if (ip.size() != 0)
            inet_pton(AF_INET6, ip.c_str(), &server_addr.sin6_addr); // 将字符串转为网络字节序
        else
            server_addr.sin6_addr = in6addr_any; // 监听所有ip地址
        server_addr.sin6_port = htons(port);
        // 绑定地址到套接字
        if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
            cerr << "绑定套接字失败\n";
            close(server_fd);
            return;
        }
    }
    // 默认：创建并绑定ipv4套接字
    else
    {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
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
        // 初始化服务器地址
        sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        // 监听特定的网络地址
        if (ip.size() != 0)
            inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr); // 将字符串转为网络字节序
        else
            server_addr.sin_addr.s_addr = INADDR_ANY; // 监听所有ip地址
        server_addr.sin_port = htons(port);
        // 绑定地址到套接字
        if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
            cerr << "绑定套接字失败\n";
            close(server_fd);
            return;
        }
    }

    // 创建向rpc注册中心连接的套接字
    sev2rpc_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in rpc_addr;
    rpc_addr.sin_family = AF_INET;
    rpc_addr.sin_port = htons(8080); // 服务器端口
    rpc_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器IP地址

    // 向本地服务表注册
    for (int i = 0; i < num; i++)
    {
        add_func_map(func[i]);
    }
    // 向注册中心注册
    for (auto it : func2idx)
    {
        // 向注册中心发起连接
        int conc = connect(sev2rpc_fd, (struct sockaddr*)&rpc_addr, sizeof(rpc_addr));
        // 向注册中心发送信息
        // 标明身份, ip, port, 注册函数名
        json registe_msg;
        registe_msg[IDENTITY] = SERVER;
        registe_msg[IP] = server_ip;
        registe_msg[PORT] = server_port;
        registe_msg[FUNC] = it.first;
        string send_msg = registe_msg.dump();
        write(conc, send_msg.c_str(), send_msg.length());
        // 接收注册中心发来的注册结果
        char buffer[1024];
        int byte = recv(conc, buffer, sizeof(buffer) - 1, 0);
        buffer[byte] = '\0';
        json ret = json::parse(buffer);
        if (ret[RET] == SUCCESS)
            cout << it.first + " 注册成功!";
        else
            cout << it.first + " 注册失败!";
        close(conc);
    }

}

void Server::server_start()
{
    thread threads[MAXTHREAD];
    // 开始监听
    listen(server_fd, SOMAXCONN);
    for (int i = 0; i < MAXTHREAD; i++)
    {
        threads[i] = thread(&Server::deal_client, this);
        threads[i].detach();
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

// TODO: 处理客户端的请求
void Server::deal_client()
{
    // 接受客户端发来的json
    // 解析请求的服务和发来的参数
    // 返回结果
}
