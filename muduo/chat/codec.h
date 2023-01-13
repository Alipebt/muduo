#include <muduo/net/TcpConnection.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>

class LengthHeaderCodec : muduo::noncopyable
{
public:
    typedef std::function<void(const muduo::net::TcpConnectionPtr &,
                               const muduo::string &message,
                               muduo::Timestamp)>
        StringMessageCallBack;
    explicit LengthHeaderCodec(const StringMessageCallBack cb)
        : messageCallback_(cb)
    {
    }

    void send(muduo::net::TcpConnection *conn,
              const muduo::StringPiece &message)
    {
        muduo::net::Buffer buf;
        buf.append(message.data(), message.size());
        int32_t len = static_cast<int32_t>(message.size());
        int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
        buf.prepend(&be32, sizeof(be32));
        conn->send(&buf);
    }

    // 收到消息后分包
    void onMessage(const muduo::net::TcpConnectionPtr &conn,
                   muduo::net::Buffer *buf,
                   muduo::Timestamp receiveTime)
    {
        while (buf->readableBytes() >= kHeaderLen) // kHeaderLen==4
        {
            const void *data = buf->peek();
            int32_t be32 = buf->peekInt32();
            const int32_t len = muduo::net::sockets::networkToHost32(be32);
            if (len > 65536 || len < 0)
            {
                LOG_INFO << "Invalid length " << len;
                conn->shutdown();
                break;
            }
            else if (buf->readableBytes() >= len + kHeaderLen)
            {
                buf->retrieve(kHeaderLen); // 从头部取走数据，删除取走部分并返回该部分
                muduo::string msg(buf->peek(), len);
                messageCallback_(conn, msg, receiveTime);
                buf->retrieve(len);
            }
            else
            {
                break;
            }
        }
    }

private:
    StringMessageCallBack messageCallback_;
    const static size_t kHeaderLen = sizeof(int32_t);
};