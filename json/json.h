// 定义了 json 报文中一些头部和对应的值
#ifndef JSONH
#define JSONH

// 服务名称, key
#define FUNC "Function"     

// 自己的身份, key
#define IDENTITY "Identity" 
// value
#define SERVER "server"
#define CLIENT "client"
#define RPC_SERVER "RPC"

// 注册或者请求结果, key
#define RET "Ret"      
// value     
#define SUCCESS "Successfully" //注册 或 申请服务成功
#define FAILD "Faild" //注册 或 申请服务失败

// ip地址, key
#define IP "Ip"             

// 端口号, key
#define PORT "Port"       

// 服务的参数
#define ARGS "Args"

#endif