#include "echo.h"

#include <muduo/base/Logging.h>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

EchoServer::EchoServer(muduo::net::EventLoop *loop,
                       const muduo::net::InetAddress &listenAddr)
    : loop_(loop),
      server_(loop, listenAddr, "EchoServer")
{
  server_.setConnectionCallback(
      std::bind(&EchoServer::onConnection, this, _1));
  server_.setMessageCallback(
      std::bind(&EchoServer::onMassage, this, _1, _2, _3));
}

void EchoServer::onConnection(const muduo::net::TcpConnectionPtr &conn)
// 在onConnection()中conn参数是TcpConnection对象的shared_ptr
{
  LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
           //  `TcpConnection`的`peerAddress()`和`localAddress()`成员函数分别返回对方和本地的地址(以`InetAddress`对象表示IP和port)
           << conn->localAddress().toIpPort() << " is "
           << (conn->connected() ? "UP" : "DOWN");
  //  `TcpConnection::connected()`返回一个bool值，表明目前链接是建立还是断开，1为链接，0为断开。
}

void EchoServer::onMassage(const muduo::net::TcpConnectionPtr &conn,
                           muduo::net::Buffer *buf,
                           muduo::Timestamp time)
// 在onMassage()中conn参数是收到数据的那个TCP链接，buf是以及收到的数据，buf的数据会积累，直到用户取走(retrieve)数据,
// time是epoll_wait()的返回时间，这个时间通常比read()发生的时间略早。
{
  muduo::string msg(buf->retrieveAllAsString());
  LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
           << "data received at " << time.toString();

  conn->send(msg);
}

void EchoServer::start()
{
  server_.start();
}