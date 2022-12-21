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
    void unregister(Observer *x);

    void notifyObservers()
    {
        for (Observer *x : observers_) // 用x遍历数组
        {
            x->update();
        }
    }

private:
    std::vector<Observer *> observers_; // 数组
};