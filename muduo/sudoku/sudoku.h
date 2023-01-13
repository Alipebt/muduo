#include <muduo/net/TcpServer.h>

class SudokuServer
{
public:
    SudokuServer(muduo::net::EventLoop *loop,
                 const muduo::net::InetAddress &listenAddr);
    void start();

private:
    void onConnection(const muduo::net::TcpConnectionPtr &conn);
    void onMessage(const muduo::net::TcpConnectionPtr &conn,
                   muduo::net::Buffer *buf,
                   muduo::Timestamp time);

private:
    muduo::net::EventLoop *loop_;
    muduo::net::TcpServer server_;
};