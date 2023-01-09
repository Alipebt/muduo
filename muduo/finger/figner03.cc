// 接受新连接后主动断开

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

using namespace muduo;
using namespace muduo::net;

void onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        conn->shutdown();
    }
}

int main()
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(1079), "Figner");
    server.setConnectionCallback(onConnection);
    server.start();
    loop.loop();
}