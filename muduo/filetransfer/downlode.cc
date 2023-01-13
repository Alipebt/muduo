/*

此方法会占用大量内存

*/

#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

const char *g_file = NULL;

string readFile(const char *filename)
{
    string content;
    FILE *fp = ::fopen(filename, "rb");
    if (fp)
    {
        // inefficient!!!
        const int kBufSize = 1024 * 1024;
        char iobuf[kBufSize];
        ::setbuffer(fp, iobuf, sizeof iobuf);

        char buf[kBufSize];
        size_t nread = 0;
        while ((nread = ::fread(buf, 1, sizeof buf, fp)) > 0)
        {
            content.append(buf, nread);
        }
        ::fclose(fp);
    }
    return content;
}

void onConnection(const TcpConnectionPtr &conn)
{
    LOG_INFO << "FileServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");

    if (conn->connected())
    {
        LOG_INFO << "FileServer - sending file " << g_file
                 << " to " << conn->peerAddress().toIpPort();
        string fileConnect = readFile(g_file);
        conn->send(fileConnect);
        conn->shutdown();
        LOG_INFO << "FileServer - done";
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
        server.start();
        loop.loop();
    }
    else
    {
        fprintf(stderr, "Usage: %s file for downlode", argv[0]);
    }
}