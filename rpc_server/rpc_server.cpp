#include "rpc_server.h"

void RPC::rpc_init(string ip, short port)
{
    // 设定身份信息
    rpc_msg[IDENTITY] = RPC_SERVER;
    // 监听套接字
    rpc_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (rpc_fd == -1)
    {
        cerr << "创建RPC注册中心套接字失败\n";
        return;
    }
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr); // 将字符串转为网络字节序
    server_addr.sin_port = htons(port);
    if (bind(rpc_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        cerr << "绑定RPC注册中心套接字失败\n";
        close(rpc_fd);
        return;
    }
    // 初始化锁
    pthread_mutex_init(&lock, NULL);
    listen(rpc_fd, SOMAXCONN);
}

// 线程的工作函数
// 需要设置成静态函数, 才可以分配给线程使用
void *RPC::worker(void *arg)
{
    // 用传参方式访问类成员函数
    RPC *my_rpc = (RPC *)arg;
    // 执行真正的线程工作逻辑
    my_rpc->rpc_deal();
    pthread_exit(NULL);
}

// 向 client 发送服务表
json RPC::rpc_dealclient(json recv_msg)
{
    json resp;
    resp[IDENTITY] = RPC_SERVER;
    pthread_mutex_lock(&lock);
    // 请求服务名称
    string func = recv_msg[FUNC];
    // 在map中查找
    int ret = func2ip.count(func);
    resp[RET] = ret ? SUC : Fa;
    if (ret)
    {
        resp[IP] = func2ip[func];
        resp[PORT] = to_string(func2port[func]);
    }
    pthread_mutex_unlock(&lock);
    // 在连接中写入信息
    return resp;
}

// 读取服务器发来的数据, 构建自己的服务表
// json信息中包含服务器的供服务调用的端口号和ip地址
json RPC::rpc_dealserver(json recv_msg)
{
    json resp;
    resp[IDENTITY] = RPC_SERVER;
    pthread_mutex_lock(&lock);
    // 需要注册的服务的名称
    string func_name = recv_msg[FUNC];
    // 注册的服务已存在, 告知客户端
    if (func2ip.count(func_name))
    {
        resp[RET] = RER;
    }
    // 注册的服务不存在, 允许注册
    else
    {
        string ip = recv_msg[IP];
        string port = recv_msg[PORT];
        // 在服务表中注册
        func2ip[func_name] = ip;
        func2port[func_name] = stoi(port);
    }
    return resp;
}

// 线程工作实际的逻辑
// 分配一个线程来处理连接, 因为每个连接会被分配一个单独的 fd, 以此实现并发处理请求
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
    if (byte == -1)
    {
        cerr << "Failed to receive message" << endl;
        close(conc);
        return;
    }
    buffer[byte] = '\0';
    recv_msg = recv_msg.parse(buffer);
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
    string msg = resp.dump();
    if (send(conc, msg.c_str(), msg.length(), 0) == -1)
    {
        cerr << "Failed to send message" << endl;
        close(conc);
        return;
    }
    close(conc);
    return;
}

void RPC::rpc_start()
{
    pthread_t thread[MAXTHREAD];
    listen(rpc_fd, SOMAXCONN);
    for (int k = 0; k < 5; k++)
    {
        for (int i = 0; i < MAXTHREAD; i++)
        {
            if (pthread_create(&thread[i], NULL, worker, this) != 0)
            {
                cerr << "Faild to creat thread\n";
                return;
            }
        }

        for (int i = 0; i < MAXTHREAD; i++)
        {
            pthread_join(thread[i], NULL);
        }
    }
}
