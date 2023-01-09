// 读取用户名,输出错误信息，然后断开连接。 如果读到\r\n结尾的消息就断开链接

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

using namespace muduo;
using namespace muduo::net;

void onMassage(const TcpConnectionPtr &conn,
               Buffer *buf,
               Timestamp time)
{
    if (buf->findCRLF())
    {
        conn->send("No such user\r\n");
        conn->shutdown();
    }
}

int main()
{
    EventLoop loop;
    TcpServer server(&loop, InetAddress(1079), "Figner");
    server.setMessageCallback(onMassage);
    server.start();
    loop.loop();
}