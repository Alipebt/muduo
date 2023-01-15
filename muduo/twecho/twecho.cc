#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>

#include <unordered_set>
#include <boost/circular_buffer.hpp>

using namespace muduo;
using namespace muduo::net;

class EchoServer : muduo::noncopyable
{
public:
    EchoServer(EventLoop *loop,
               const InetAddress &listenAddr,
               int idleSecond)
        : loop_(loop),
          server_(loop, listenAddr, "EchoServer"),
          connectionBuckets_(idleSecond)
    {
        server_.setConnectionCallback(
            std::bind(&EchoServer::onConnection, this, _1));
        server_.setMessageCallback(
            std::bind(&EchoServer::onMessage, this, _1, _2, _3));
        loop_->runEvery(3.0,
                        std::bind(&EchoServer::onTime, this));
        connectionBuckets_.resize(idleSecond);
    }

    void start()
    {
        server_.start();
    }

    void onConnection(const TcpConnectionPtr &conn)
    {
        LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort()
                 << " -> " << conn->localAddress().toIpPort() << " is "
                 << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected())
        {
            EntryPtr entry(new Entry(conn));
            connectionBuckets_.back().insert(entry);

            WeakEntryPtr weakEntry(entry);
            conn->setContext(weakEntry);
        }
        else
        {
            assert(!conn->getContext().empty());
            WeakEntryPtr weakEntry(boost::any_cast<WeakEntryPtr>(conn->getContext()));
            LOG_INFO << "Entry use_count = " << weakEntry.use_count();
        }
    }

    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp time)
    {
        string msg(buf->retrieveAllAsString());
        LOG_INFO << conn->name() << " echo " << msg.size()
                 << " bytes at " << time.toString();
        conn->send(msg);

        assert(!conn->getContext().empty());
        WeakEntryPtr weakEntry(boost::any_cast<WeakEntryPtr>(conn->getContext()));
        EntryPtr entry(weakEntry.lock());
        if (entry)
        {
            connectionBuckets_.back().insert(entry);
            dumpConnectionBuckets();
        }
    }

    void onTime()
    {
        connectionBuckets_.push_back(Bucket());
        dumpConnectionBuckets();
    }

    void dumpConnectionBuckets() const
    {
        LOG_INFO << "size = " << connectionBuckets_.size();
        int idx = 0;
        for (WeakConnectionList::const_iterator bucketI = connectionBuckets_.begin();
             bucketI != connectionBuckets_.end();
             ++bucketI, ++idx)
        {
            const Bucket &bucket = *bucketI;
            printf("[%d] len = %zd : ", idx, bucket.size());
            for (const auto &it : bucket)
            {
                bool connectionDead = it->weakConn_.expired();
                printf("%p(%ld)%s, ", get_pointer(it), it.use_count(),
                       connectionDead ? " DEAD" : "");
            }
            puts("");
        }
    }

private:
    typedef std::weak_ptr<TcpConnection> WeakTcpConnectionPtr;

    class Entry : muduo::copyable
    {
    public:
        explicit Entry(const WeakTcpConnectionPtr &weakConn)
            : weakConn_(weakConn)
        {
        }

        ~Entry()
        {
            TcpConnectionPtr conn = weakConn_.lock();
            if (conn)
            {
                conn->shutdown();
            }
        }

        WeakTcpConnectionPtr weakConn_;
    };

    typedef std::shared_ptr<Entry> EntryPtr;
    typedef std::weak_ptr<Entry> WeakEntryPtr;
    typedef std::unordered_set<EntryPtr> Bucket;
    typedef boost::circular_buffer<Bucket> WeakConnectionList;

    EventLoop *loop_;
    TcpServer server_;
    WeakConnectionList connectionBuckets_;
};

int main(int argc, char *argv[])
{

    EventLoop loop;
    InetAddress listenAddr(2007);
    int idleSeconds = 10;
    if (argc > 1)
    {
        idleSeconds = atoi(argv[1]);
    }
    LOG_INFO << "pid = " << getpid() << ", idle seconds = " << idleSeconds;
    EchoServer server(&loop, listenAddr, idleSeconds);
    server.start();
    loop.loop();
}