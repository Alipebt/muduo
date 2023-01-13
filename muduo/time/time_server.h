#include <muduo/net/TcpServer.h>

using namespace muduo;
using namespace muduo::net;

class TimeServer
{
public:
    TimeServer(EventLoop *loop,
               const InetAddress &listenAddr);
    void start();

private:
    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp time);

private:
    EventLoop *loop_;
    TcpServer server_;
};