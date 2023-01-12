#include <muduo/time/time_client.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>

TimeClient::TimeClient(EventLoop *loop,
                       const InetAddress &serverAddr)
    : loop_(loop),
      client_(loop, serverAddr, "TimeClient")
{
    client_.setConnectionCallback(
        std::bind(&onConnection, this, _1));
    client_.setMessageCallback(
        std::bind(&onMassage, this, _1, _2, _3));
};

void TimeClient::connect()
{
    client_.connect();
}

void TimeClient::onConnection(const TcpConnectionPtr &conn)
{
    LOG_INFO << conn->localAddress().toIpPort() << " -> "
             << conn->peerAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
    if (!conn->connected())
    {
        loop_->quit();
    }
}

void TimeClient::onMassage(const TcpConnectionPtr &conn,
                           Buffer *buf,
                           Timestamp receiveTime)
{
    if (buf->readableBytes() >= sizeof(int32_t))
    {
        const void *data = buf->peek();
        int32_t be32 = *static_cast<const int32_t *>(data);
        buf->retrieve(sizeof(int32_t));
        time_t time = sockets::networkToHost32(be32);
        Timestamp ts(time * Timestamp::kMicroSecondsPerSecond);
        LOG_INFO << "Server time = " << time << ", " << ts.toFormattedString();
    }
    else
    {
        LOG_INFO << conn->name() << "no enough data" << buf->readableBytes()
                 << " at " << receiveTime.toFormattedString();
    }
}

int main(int argc, char *argv[])
{
    LOG_INFO << "pid = " << getpid();
    if (argc > 1)
    {
        EventLoop loop;
        InetAddress serverAddr(argv[1], 2037);

        TimeClient timeClient(&loop, serverAddr);
        timeClient.connect();
        loop.loop();
    }
    else
    {
        printf("Usage: %s host_ip\n", argv[0]);
    }
}