#include "server.h"
/// @brief 初始化 sever 端, 包括设置基本参数, 注册服务以及绑定套接字
/// @param ip sever端监听的ip地址
/// @param port sever端的端口
/// @param num 需要注册的服务个数
/// @param rpc_ip 注册中心的ip地址
/// @param rpc_port 注册中心的端口
/// @param ipv6 是否监听ipv6地址, 默认否
void Server::server_init(string ip, short port, int num,
                         string rpc_ip, short rpc_port, int ipv6)
{
    // 设置服务器基本参数
    num_func = num;
    server_ip = ip;
    server_port = port;
    // 创建并绑定ipv6套接字
    if (ipv6)
    {
        with_clt_fd = socket(AF_INET6, SOCK_STREAM, 0);
        if (with_clt_fd == -1)
        {
            cerr << "创建套接字失败\n";
            return;
        }
        // 允许地址重用
        int reuse = 1;
        if (setsockopt(with_clt_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
        {
            cerr << "设置地址重用失败" << endl;
            close(with_clt_fd);
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
        if (bind(with_clt_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
            cerr << "绑定套接字失败\n";
            close(with_clt_fd);
            return;
        }
    }
    // 默认：创建并绑定ipv4套接字
    else
    {
        with_clt_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (with_clt_fd == -1)
        {
            cerr << "创建套接字失败\n";
            return;
        }
        // 允许地址重用
        int reuse = 1;
        if (setsockopt(with_clt_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
        {
            cerr << "设置地址重用失败" << endl;
            close(with_clt_fd);
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
        if (bind(with_clt_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
            cerr << "绑定套接字失败\n";
            close(with_clt_fd);
            return;
        }
    }
    // 向本地服务表注册
    for (int i = 0; i < num; i++)
    {
        int idx;
        cin >> idx;
        cout << "输入服务编号: ";
        add_func_map(idx);
        cout << "注册成功!(本地服务表)\n";
    }

    // 创建向rpc注册中心连接的套接字
    struct sockaddr_in rpc_addr;
    rpc_addr.sin_family = AF_INET;
    rpc_addr.sin_port = htons(rpc_port);                  // 服务器端口
    rpc_addr.sin_addr.s_addr = inet_addr(rpc_ip.c_str()); // 服务器IP地址
    // 向注册中心注册
    for (auto it : func2idx)
    {
        with_rpc_fd = socket(AF_INET, SOCK_STREAM, 0);
        set_timeout(with_rpc_fd, 10); // 设置读写超时时间为10秒
        // 向注册中心发起连接
        int conc = -1;
        while (conc == -1)
        {
            conc = connect(with_rpc_fd, (struct sockaddr *)&rpc_addr, sizeof(rpc_addr));
        };
        // 向注册中心发送信息
        // 标明身份, ip, port, 注册函数名
        json registe_msg;
        registe_msg[IDENTITY] = SERVER;
        registe_msg[IP] = server_ip;
        registe_msg[PORT] = server_port;
        registe_msg[FUNC] = it.first;
        string send_msg = registe_msg.dump();
        // 一直写直到写成功
        while (send(with_rpc_fd, send_msg.c_str(), send_msg.length(), 0) == -1)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                cout << "write超时" << endl;
            }
            else
            {
                cout << "write失败" << endl;
            }
            close(conc);
            close(with_rpc_fd);
            sleep(3);
        }
        // 接收注册中心发来的注册结果
        char buffer[1024];
        int byte = -1;
        // 处理读失败
        if (byte == -1)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                cout << "recv超时" << endl;
            }
            else
            {
                cout << "recv失败" << endl;
            }
            close(conc);
            close(with_rpc_fd);
            continue;
        }
        buffer[byte] = '\0';
        json ret = json::parse(buffer);
        if (ret[RET] == SUCCESS)
            cout << it.first + " 注册成功!(注册中心)\n";
        else
            cout << it.first + " 注册失败!(注册中心)\n";
        close(conc);
        close(with_rpc_fd);
    }
}

/// @brief sever 端开始监听
void Server::server_start()
{
    thread threads[MAXTHREAD];
    // 开始监听
    listen(with_clt_fd, SOMAXCONN);
    for (int i = 0; i < MAXTHREAD; i++)
    {
        threads[i] = thread(&Server::deal_client, this);
        threads[i].detach();
    }
    while (true)
    {
        string s;
        cout << "输入 SHUT DOWN 以关闭服务器\n";
        getline(cin, s);
        transform(s.begin(), s.end(), s.begin(), ::toupper);
        if (s == "SHUT DOWN")
        {
            is_stop = true;
            return;
        }
    }
}

/// @brief 向本地服务表注册服务
/// @param func_idx 服务编号
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

/// @brief 处理客户端的请求
void Server::deal_client()
{
    while (!is_stop)
    {
        // 接受客户端发来的json
        int conc = accept(with_clt_fd, nullptr, nullptr);
        if (conc == -1)
        {
            cout << "Accept失败" << endl;
            continue;
        }
        set_timeout(conc, 10); // 设置读写超时时间为10秒
        char buffer[1024];
        int byte = recv(conc, buffer, sizeof(buffer) - 1, 0);
        // 处理读失败
        if (byte == -1)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                cout << "recv超时" << endl;
            }
            else
            {
                cout << "recv失败" << endl;
            }
            close(conc);
            continue;
        }
        buffer[byte] = '\0';
        json recv_msg = json::parse(buffer);
        json send_msg;
        // 解析请求的服务和发来的参数
        string func_name = recv_msg[FUNC]; // 请求的服务名称
        string clt_args = recv_msg[ARGS];  // 所有参数
        // 把参数分割开
        regex re(" ");
        sregex_token_iterator it(clt_args.begin(), clt_args.end(), re, -1);
        sregex_token_iterator end;
        vector<string> all_args(it, end);
        auto item = func2idx.find(func_name);
        // 服务不存在
        if (item == func2idx.end())
        {
            send_msg[RET] = FAILD;
        }
        else
        {
            int func_idx = item->second;
            switch (func_idx)
            {
            case 0:
                send_msg[RES] = Server::add(stod(all_args[0]), stod(all_args[1]));
                break;
            case 1:
                send_msg[RES] = Server::subtract(stod(all_args[0]), stod(all_args[1]));
                break;
            case 2:
                send_msg[RES] = Server::multiply(stod(all_args[0]), stod(all_args[1]));
                ;
                break;
            case 3:
                send_msg[RES] = Server::divide(stod(all_args[0]), stod(all_args[1]));
                break;
            case 4:
                send_msg[RES] = Server::concat(all_args[0], all_args[1]);
                break;
            case 5:
                send_msg[RES] = Server::compare(stod(all_args[0]), stod(all_args[1]));
                break;
            case 6:
                send_msg[RES] = Server::func1();
                break;
            case 7:
                send_msg[RES] = Server::func2();
                break;
            case 8:
                send_msg[RES] = Server::func3();
                break;
            case 9:
                send_msg[RES] = Server::func4();
                break;
            case 10:
                send_msg[RES] = Server::func5();
                break;
            case 11:
                send_msg[RES] = Server::func6();
                break;
            default:
                send_msg[RET] = FAILD;
                break;
            }
        }
        // 返回结果
        send_msg[RET] = SUCCESS;
        string msg = send_msg.dump();
        if (send(conc, msg.c_str(), msg.length(), 0) == -1)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                cout << "write超时" << endl;
            }
            else
            {
                cout << "write失败" << endl;
            }
            close(conc);
            continue;
        }
        close(conc);
    }

    return;
}

/// @brief 设置连接读写超时
/// @param fd 连接的描述符
/// @param sec 超时时间(秒)
void Server::set_timeout(int fd, int sec)
{
    timeval tv = {sec, 0};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv)); // 设置接受超时
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof(tv)); // 设置发送超时
}

Server::~Server()
{
    close(with_clt_fd);
}
