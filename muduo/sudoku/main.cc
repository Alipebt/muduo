#include "sudoku.h"

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

int main()
{
    LOG_INFO << "pid = " << getpid();
    muduo::net::EventLoop loop;
    muduo::net::InetAddress listenAddr(2007);
    SudokuServer server(&loop, listenAddr);
    server.start();
    loop.loop();
}