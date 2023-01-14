#include <muduo/net/EventLoop.h>
#include <iostream>

using namespace muduo;
using namespace muduo::net;

class Printer : muduo::noncopyable
{
public:
    Printer(EventLoop *loop)
        : loop_(loop)
    {

        loop_->runAfter(1,
                        std::bind(&Printer::print, this));
    }
    ~Printer()
    {
        std::cout << "Final count is " << count_ << "\n";
    }

    void print()
    {

        if (count_ < 5)
        {

            std::cout << count_ << "\n";
            ++count_;

            loop_->runAfter(1,
                            std::bind(&Printer::print, this));
        }
        else
        {
            loop_->quit();
        }
    }

private:
    EventLoop *loop_;
    int count_;
};

int main()
{
    EventLoop loop;
    Printer printer(&loop);
    loop.loop();
}