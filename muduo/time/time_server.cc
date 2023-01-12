#include <muduo/time/time_server.h>
#include <muduo/base/Logging.h>

TimeServer::TimeServer(EventLoop *loop,
                       const InetAddress &listenAddr)
    : loop_(loop),
      server_(loop, listenAddr, "TimeServer")
{
    server_.setConnectionCallback(
        std::bind(&onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&onMassage, this, _1, _2, _3));
}

void TimeServer::start()
{
    server_.start();
}

void TimeServer::onConnection(const TcpConnectionPtr &conn)
{
    LOG_INFO << "TimeServer- " << conn->peerAddress().toIpPort() << "->"
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
    if (conn->connected())
    {
        time_t now = ::time(NULL); // ::直接用在全局函数前，表示是全局函数
        int32_t be32 = sockets::hostToNetwork32(static_cast<int32_t>(now));
        conn->send(&be32, sizeof(be32));
        conn->shutdown();
    }
}

void TimeServer::onMassage(const TcpConnectionPtr &conn,
                           Buffer *buf,
                           Timestamp time)
{
    muduo::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " retrieve " << msg.size() << " bytes ,"
             << " date received at " << time.toString();
}