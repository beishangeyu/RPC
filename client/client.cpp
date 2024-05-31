#include "client.h"

void Client::set_timeout(int fd, int sec)
{
    timeval tv = {sec, 0};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv)); // 设置接受超时
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof(tv)); // 设置发送超时
}

/// @brief 初始化客户端参数
/// @param rpc_ip 注册中心ip地址
/// @param rpc_port 注册中心端口号
void Client::client_init(string rpc_ip, short rpc_port)
{
    this->rpc_ip = rpc_ip;
    this->rpc_port = rpc_port;
}

/// @brief 客户端开始运行
void Client::client_start()
{
    while (!is_stop)
    {
        string func;
        cout << "输入想要获取的服务名称, 输入shut down关闭\n";
        cin >> func;
        if (func == "shut down")
        {
            is_stop = true;
            sleep(3);
            return;
        }
        if (!client_pull(func))
        {
            cerr << "向注册中心请求服务失败\n";
            continue;
        }
        else
        {
            cout << "向注册中心请求服务成功\n";
        }
        if (!client_call(func))
        {
            cerr << "服务端调用服务失败\n";
            continue;
        }
        else
        {
            cout << "服务端调用服务成功\n";
        }
    }
}

/// @brief 向注册中心请求服务器地址
/// @param func 请求的服务
/// @return 请求结果, 0表示失败, 1表示成功
int Client::client_pull(string func)
{
    // 编写向注册中心发送的信息
    json send2rpc;
    send2rpc[IDENTITY] = CLIENT;
    send2rpc[FUNC] = func;
    // 创建套接字和注册中心通信
    with_rpc = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in rpc_addr;
    rpc_addr.sin_family = AF_INET;
    rpc_addr.sin_port = htons(rpc_port);
    inet_pton(AF_INET, rpc_ip.c_str(), &rpc_addr.sin_addr);
    int conc = -1;
    // 尝试10次, 还不行就报错
    for (int i = 0; i < 10 && conc == -1; i++)
    {
        conc = connect(with_rpc, (struct sockaddr *)&rpc_addr, sizeof(rpc_addr));
        sleep(3);
    }
    if (conc == -1)
    {
        return 0;
    }
    set_timeout(with_rpc, 10);
    // 发送信息
    string s = send2rpc.dump();
    send(with_rpc, s.c_str(), s.length(), 0);
    // 接收请求结果
    char buffer[1024];
    int byte = recv(with_rpc, buffer, sizeof(buffer) - 1, 0);
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
        return 0;
    }
    close(conc);
    buffer[byte] = '\0';
    json recv_msg = json::parse(buffer);
    // 请求成功
    if (recv_msg[RET] == SUCCESS)
    {
        server_ip = recv_msg[IP];
        server_port = recv_msg[PORT];
        return 1;
    }
    // 请求失败
    else
    {
        return 0;
    }
}

/// @brief 向服务端调用服务
/// @param func 调用的服务名称
/// @return 调用结果, 0失败, 1成功
int Client::client_call(string func)
{
    // 读入参数
    string args;
    cout << "输入参数, 参数间以空格分割\n";
    cin >> args;
    // 编写向 sever 端发送的信息
    json send2ser;
    send2ser[IDENTITY] = CLIENT;
    send2ser[FUNC] = func;
    send2ser[ARGS] = args;
    // 创建和sever端通信的套接字
    with_ser = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(rpc_port);
    inet_pton(AF_INET, rpc_ip.c_str(), &server_addr.sin_addr);
    int conc = -1;
    // 尝试10次, 还不行就报错
    for (int i = 0; i < 10 && conc == -1; i++)
    {
        conc = connect(with_ser, (struct sockaddr *)&server_addr, sizeof(server_addr));
        sleep(3);
    }
    if (conc == -1)
    {
        return 0;
    }
    set_timeout(with_ser, 10);
    // 发送信息
    string s = send2ser.dump();
    send(with_ser, s.c_str(), s.length(), 0);
    // 接收请求结果
    char buffer[1024];
    int byte = recv(with_ser, buffer, sizeof(buffer) - 1, 0);
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
        return 0;
    }
    close(conc);
    buffer[byte] = '\0';
    json recv_msg = json::parse(buffer);
    // 调用成功, 输出运行结果
    if (recv_msg[RET] == SUCCESS)
    {
        cout << "运行结果: " << string(recv_msg[RES]) << endl;
        return 1;
    }
    // 调用失败
    else
    {
        return 0;
    }
}
