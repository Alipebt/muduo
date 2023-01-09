// 向UserMap添加一个用户。

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <map>

using namespace muduo;
using namespace muduo::net;

typedef std::map<string, string> UserMap;
UserMap users;

string getUser(const string &user)
{
    string result = "No such user";
    UserMap::iterator it = users.find(user);
    if (it != users.end())
    {
        result = it->second;
        // it->first是key,it->second是value
    }
    return result;
}

void onMassage(const TcpConnectionPtr &conn,
               Buffer *buf,
               Timestamp time)
{

    const char *crlf = buf->findCRLF();
    // Buffer类通过findCRLF和findEOL成员函数在readerable区域检索\r\n和EOL,返回检索到的地址
    if (crlf)
    {
        string user(buf->peek(), crlf);
        conn->send(getUser(user) + "\r\n");
        buf->retrieveUntil(crlf + 2);
        conn->shutdown();
    }
}

int main()
{
    users["schen"] = "Happy and well";
    EventLoop loop;
    TcpServer server(&loop, InetAddress(1079), "Figner");
    server.setMessageCallback(onMassage);
    server.start();
    loop.loop();
}