#include <pthread.h>
#include <assert.h>
// #include <muduo/base/Mutex.h>
#include <muduo/base/CurrentThread.h>

class MutexLock
{
public:
    MutexLock()
    {
        pthread_mutex_init(&m_mutex, NULL);
        holder_ = 0;
    }
    ~MutexLock()
    {
        assert(holder_ == 0);
        pthread_mutex_destroy(&m_mutex);
    }
    bool isLockedByThisThread()
    {
        return holder_ == muduo::CurrentThread::tid();
    }
    void assertLocked()
    {
        assert(isLockedByThisThread());
    }
    void lock()
    {
        pthread_mutex_lock(&m_mutex);
        holder_ = muduo::CurrentThread::tid();
    }
    void unlock()
    {
        holder_ = 0;
        pthread_mutex_unlock(&m_mutex);
    }
    pthread_mutex_t *getPthreadMutex()
    {
        return &m_mutex;
    }

private:
    pthread_mutex_t m_mutex;
    pid_t holder_;
};
