#include <MutexLock.h>
class CountDownLacth : boost::noncopyable
{
public:
    explicit CountDownLacth(int count) : mutex_(),
                                         condition_(mutex_), // 初始化顺序要和成员声明保持一致
                                         count_(count)
    {
    }
    void wait()
    {
        MutexLockGuard lock(mutex_);
        while (count_ > 0)
        {
            condition_.wait();
        }
    }
    void countDown()
    {
        MutexLockGuard lock(mutex_);
        --count_;
        if (count_ == 0)
        {
            condition_.notifyAll();
        }
    }

private:
    mutable MutexLock mutex_; // mutex必须先于condition
    Condition condition_;
    int count_;
};