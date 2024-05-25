#include "rpc_server.h"

/// @brief 初始化注册中心
/// @param ip 注册中心监听的ip地址
/// @param port 注册中心监听的端口
void Rpc_server::rpc_init(string ip, short port)
{
    // 监听套接字
    rpc_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (rpc_fd == -1)
    {
        cerr << "创建套接字失败\n";
        return;
    }
    // 允许端口复用
    int reuse = 1;
    if (setsockopt(rpc_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        cerr << "设置地址重用失败" << endl;
        close(rpc_fd);
        return;
    }
    // 绑定地址到套接字
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr); // 将字符串转为网络字节序
    server_addr.sin_port = htons(port);
    if (bind(rpc_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        cerr << "绑定套接字失败\n";
        close(rpc_fd);
        return;
    }
}

/// @brief 将 client 请求的服务所在服务器地址发给 client
/// @param recv_msg client 发来的消息
/// @return 查询结果和地址
json Rpc_server::rpc_dealclient(json recv_msg)
{
    json resp;
    resp[IDENTITY] = RPC_SERVER;
    lock.lock();
    // 请求服务名称
    string func = recv_msg[FUNC];
    // 在map中查找
    int ret = func2ip.count(func);
    resp[RET] = ret ? SUCCESS : FAILD;
    if (ret)
    {
        resp[IP] = func2ip[func];
        resp[PORT] = to_string(func2port[func]);
    }
    lock.unlock();
    return resp;
}

/// @brief 处理 sever 注册服务的请求
/// @param recv_msg server 发来的信息
/// @return 注册结果
json Rpc_server::rpc_dealserver(json recv_msg)
{
    json resp;
    resp[IDENTITY] = RPC_SERVER;

    // 需要注册的服务的名称
    string func_name = recv_msg[FUNC];
    lock.lock();
    // 注册的服务已存在, 告知客户端
    if (func2ip.count(func_name))
    {
        // 注册结果: 重复注册
        resp[RET] = FAILD;
    }
    // 注册的服务不存在, 允许注册
    else
    {
        string ip = recv_msg[IP];
        short port = recv_msg[PORT];
        // 在服务表中注册
        func2ip[func_name] = ip;
        func2port[func_name] = port;
        // 注册结果:成功
        resp[RET] = SUCCESS;
    }
    lock.unlock();
    return resp;
}

/// @brief 处理 connect 请求, 线程的工作函数
void Rpc_server::rpc_deal()
{
    while (!is_stop)
    {
        int conc = accept(rpc_fd, nullptr, nullptr);
        // 处理获取连接失败的情况
        if (conc == -1)
        {
            cout << "Accept失败" << endl;
            continue;
        }
        set_timeout(conc, 20); // 设置读写超时时间为10秒
        cout << conc << endl;
        char buffer[1024];
        // 从连接上读取数据
        json recv_msg;
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
        recv_msg = recv_msg.parse(buffer);
        cout << recv_msg;
        // 生成响应报文并写入连接
        json resp;
        if (recv_msg[IDENTITY] == CLIENT)
        {
            resp = rpc_dealclient(recv_msg);
        }
        else
        {
            resp = rpc_dealserver(recv_msg);
        }
        // 把 json 转化为字节流, 往连接上写数据
        string msg = resp.dump();
        // 处理写失败
        if (write(conc, msg.c_str(), msg.length()) == -1)
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

/// @brief 为连接设置超时时间
/// @param fd 连接的文件描述符
/// @param sec 超时时间(秒)
void Rpc_server::set_timeout(int fd, int sec)
{
    timeval tv = {sec, 0};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv)); // 设置接受超时
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof(tv)); // 设置发送超时
}

void Rpc_server::rpc_start()
{
    thread threads[MAXTHREAD];
    // 开始监听
    listen(rpc_fd, SOMAXCONN);
    for (int i = 0; i < MAXTHREAD; i++)
    {
        // 让线程去处理连接
        // 通过传入this指针, 让类的成员函数可以创建线程, 线程的工作函数也是类的成员函数
        threads[i] = thread(&Rpc_server::rpc_deal, this);
        threads[i].detach(); // 让线程独立运行
    }
    while (true)
    {
        string s;
        cout << "输入 SHUT DOWN 以关闭注册中心\n";
        getline(cin, s);
        transform(s.begin(), s.end(), s.begin(), ::toupper);
        if (s == "SHUT DOWN")
            is_stop = true;
        return;
    }
}

Rpc_server::~Rpc_server()
{
    // 关闭监听套接字
    close(rpc_fd);
    // 销毁锁
    lock.~mutex();
}
