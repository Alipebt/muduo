/*

同downlode2
downlode3避免了手动调用::fclose()

*/

#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

void onHighWaterMark(const TcpConnectionPtr &conn, size_t len)
{
    LOG_INFO << "HighWaterMark " << len;
}

const int kBufSize = 64 * 1024;
const char *g_file = NULL;
typedef std::shared_ptr<FILE> FilePtr;

void onConnection(const TcpConnectionPtr &conn)
{
    LOG_INFO << "FileServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");

    if (conn->connected())
    {
        LOG_INFO << "FileServer - sending file " << g_file
                 << " to " << conn->peerAddress().toIpPort();

        conn->setHighWaterMarkCallback(onHighWaterMark, kBufSize + 1);
        FILE *fp = ::fopen(g_file, "rb");
        if (fp)
        {
            FilePtr ctx(fp, ::fclose);
            conn->setContext(ctx);
            char buf[kBufSize];
            size_t nread = ::fread(buf, 1, kBufSize, fp);
            conn->send(buf, nread); // 存在WriteCompleteCallBack,在此重复写文件。
        }
        else
        {
            conn->shutdown();
            LOG_INFO << "FileServer - no such file";
        }
    }
}

void onWriteComplete(const TcpConnectionPtr &conn)
{
    const FilePtr &fp = boost::any_cast<const FilePtr &>(conn->getContext());
    char buf[kBufSize];
    size_t nread = ::fread(buf, 1, kBufSize, get_pointer(fp));
    if (nread > 0)
    {
        conn->send(buf, nread);
    }
    else
    {
        conn->shutdown();
        LOG_INFO << "FileServer - done ";
    }
}

int main(int argc, char *argv[])
{
    LOG_INFO << "pid = " << getpid();
    if (argc > 1)
    {
        g_file = argv[1];

        EventLoop loop;
        InetAddress listenAddr(2021);
        TcpServer server(&loop, listenAddr, "FileServer");
        server.setConnectionCallback(onConnection);
        server.setWriteCompleteCallback(onWriteComplete);
        server.start();
        loop.loop();
    }
    else
    {
        fprintf(stderr, "Usage: %s file for downlode", argv[0]);
    }
}