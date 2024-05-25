#include "server.h"
#include "server.cpp"
using namespace std;

int main(int argc, char *argv[])
{
    Server my_server;
    int fun[3] = {0, 1, 2};
    my_server.server_init("127.0.0.1", 8080, 3, fun, "127.0.0.1", 8000);
    my_server.server_start();
}