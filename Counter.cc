#include <boost/noncopyable.hpp>
#include <memory>
#include <vector>
using namespace std;

class Observer
{
public:
    virtual ~Observer();
    virtual void update() = 0;
};

class Observable
{
public:
    void register_(weak_ptr<Observer> x);

    void notifyObservers();

private:
    mutable int mutex_;
    std::vector<weak_ptr<Observer>> observers_; // 数组
    typedef std::vector<weak_ptr<Observer>>::iterator Iterator;
};

void Observable::notifyObservers()
{
    mutex_ = 1;
    Iterator it = observers_.begin();
    while (it != observers_.end())
    {
        shared_ptr<Observer> obj(it->lock());
        if (obj)
        {
            obj->update();
            ++it;
        }
        else
        {
            it = observers_.erase(it);
        }
    }
}