#include <pthread.h>
#include <assert.h>
#include <boost/noncopyable.hpp>
#include <muduo/base/CurrentThread.h>

class MutexLock : boost::noncopyable
{
public:
    MutexLock() : holder_(0)
    {
        pthread_mutex_init(&mutex_, NULL);
    }
    ~MutexLock()
    {
        assert(holder_ == 0);
        pthread_mutex_destroy(&mutex_);
    }
    bool isLockedByThisThread()
    {
        return holder_ == muduo::CurrentThread::tid();
    }
    void assertLocked()
    {
        assert(isLockedByThisThread());
    }
    void lock() // 仅供MutexLockGuard使用，严禁用户代码调用
    {
        pthread_mutex_lock(&mutex_); // 这两行不能反
        holder_ = muduo::CurrentThread::tid();
    }
    void unlock() // 仅供MutexLockGuard使用，严禁用户代码调用
    {
        holder_ = 0; // 这两行不能反
        pthread_mutex_unlock(&mutex_);
    }
    pthread_mutex_t *getPthreadMutex() // 仅供Condition使用，严禁用户代码调用
    {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_;
    pid_t holder_;
};

class MutexLockGuard : boost::noncopyable
{
public:
    explicit MutexLockGuard(MutexLock &mutex) : mutex_(mutex)
    {
        mutex_.lock();
    }
    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:
    MutexLock &mutex_;
};

#define MutexLockGuard(x) static_assert(false, "missing mutex guard var name")

class Condition : boost::noncopyable
{
public:
    explicit Condition(MutexLock &mutex) : mutex_(mutex)
    {
        pthread_cond_init(&pcond_, NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }

    void wait()
    {
        pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }

    void notify()
    {
        pthread_cond_signal(&pcond_);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&pcond_);
    }

private:
    MutexLock &mutex_;
    pthread_cond_t pcond_;
};
