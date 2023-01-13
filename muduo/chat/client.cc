#include "codec.h"

#include <muduo/net/TcpClient.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoopThread.h>

#include <iostream>

using namespace muduo;
using namespace muduo::net;
using namespace std;

class ChatClient : muduo::noncopyable
{
public:
    ChatClient(EventLoop *loop,
               const InetAddress &serverAddr)
        : loop_(loop),
          client_(loop, serverAddr, "ChatClient"),
          codec_(
              std::bind(&ChatClient::onStringMessage, this, _1, _2, _3))
    {
        client_.setConnectionCallback(
            std::bind(&ChatClient::onConnection, this, _1));
        client_.setMessageCallback(
            std::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
        client_.enableRetry();
    }
    void connect()
    {
        client_.connect();
    }
    void disconnect()
    {
        client_.disconnect();
    }
    void write(const StringPiece &message)
    {
        MutexLockGuard lock(mutex_);

        if (connection_)
        {
            codec_.send(get_pointer(connection_), message);
        }
    }

private:
    void onConnection(const TcpConnectionPtr &conn)
    {
        LOG_INFO << conn->localAddress().toIpPort() << " -> "
                 << conn->peerAddress().toIpPort() << " is "
                 << (conn->connected() ? "UP" : "DOWN");
        MutexLockGuard lock(mutex_);
        if (conn->connected())
        {
            connection_ = conn;
        }
        else
        {
            connection_.reset();
        }
    }

    void onStringMessage(const TcpConnectionPtr &,
                         const string &message,
                         Timestamp)
    {
        printf("<<< %s\n", message.c_str());
    }

private:
    EventLoop *loop_;
    TcpClient client_;
    TcpConnectionPtr connection_;
    MutexLock mutex_;
    LengthHeaderCodec codec_;
};

int main(int argc, char *argv[])
{
    LOG_INFO << "pid = " << getpid();
    if (argc > 2)
    {
        EventLoopThread loopThread;
        int32_t port = static_cast<int32_t>(atoi(argv[2]));
        InetAddress serverAddr(argv[1], port);

        ChatClient client(loopThread.startLoop(), serverAddr);
        client.connect();

        string line;
        while (getline(cin, line))
        {
            client.write(line);
        }
        client.disconnect();
    }
    else
    {
        printf("Usage : %s host_ip port\n", argv[0]);
    }
}