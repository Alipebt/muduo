#include "sudoku.h"

#include <muduo/base/Logging.h>
#include <muduo/base/Types.h>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

using namespace std;

const int kCells = 81;

string solveSudoku(string request)
{
  string solved = request;
  return solved;
}

SudokuServer::SudokuServer(muduo::net::EventLoop *loop,
                           const muduo::net::InetAddress &listenAddr)
    : loop_(loop),
      server_(loop, listenAddr, "SudokuServer")
{
  server_.setConnectionCallback(
      std::bind(&SudokuServer::onConnection, this, _1));
  server_.setMessageCallback(
      std::bind(&SudokuServer::onMessage, this, _1, _2, _3));
}

void SudokuServer::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
  LOG_INFO << "SudokuServer - " << conn->peerAddress().toIpPort() << " -> "
           << conn->localAddress().toIpPort() << " is "
           << (conn->connected() ? "UP" : "DOWN");
}

void SudokuServer::onMessage(const muduo::net::TcpConnectionPtr &conn,
                             muduo::net::Buffer *buf,
                             muduo::Timestamp time)
{
  LOG_DEBUG << conn->name();
  size_t len = buf->readableBytes();
  while (len > kCells + 2) // 反复读取数据，2为回车换行符
  {
    const char *crlf = buf->findCRLF();
    if (crlf) // 如果找到一条完整的请求（检查是否换行）
    {
      string request(buf->peek(), crlf); // 取出请求,buf->peek()指向当前字符
      string id;
      buf->retrieveUntil(crlf + 2); // 取回已读取的数据
      LOG_INFO << "request: " << request << "\t"
               << "retrieve: " << buf->peek();
      string::iterator colon = find(request.begin(), request.end(), ':');
      if (colon != request.end()) // 如果找到id部分
      {
        id.assign(request.begin(), colon);         // 函数assign()常用在给string类变量赋值.
        request.erase(request.begin(), colon + 1); // 函数erase()常用在处理删除字符串,返回删除后的字符串
      }
      if (request.size() == muduo::implicit_cast<size_t>(kCells)) // 类型转换<目标类型>（被转换类型）
      {
        string result = solveSudoku(request); // 求解数独
        if (id.empty())
        {
          conn->send(result + "\r\n");
        }
        else
        {
          conn->send(id + ":" + result + "\r\n");
        }
      }
      else
      {
        conn->send("Bad Request!\r\n");
        conn->shutdown();
      }
    }
    else // 请求不完整
    {
      break;
    }
  }
}

void SudokuServer::start()
{
  server_.start();
}
