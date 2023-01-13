#include <muduo/net/TcpClient.h>
#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;

class TimeClient
{
public:
    TimeClient(EventLoop *loop,
               const InetAddress &serverAddr);
    void connect();

private:
    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp time);

private:
    EventLoop *loop_;
    TcpClient client_;
};