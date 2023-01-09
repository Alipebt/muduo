// 接受新连接。在1079端口侦听新连接，接受之后什么都不做，程序空等。muduo会自动丢弃收到的数据。

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

using namespace muduo;
using namespace muduo::net;

int main()
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(1079), "Figner");
    server.start();
    loop.loop();
}