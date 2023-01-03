#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
// #include <boost/shared_ptr.hpp>
#include <map>
using namespace std;

class Stock
{
public:
    Stock(const string &newkey)
        : key(newkey), IBM("new")
    {
    }

public:
    string key;

private:
    string IBM;
};

class StockFactory : public boost::enable_shared_from_this<StockFactory>, // 让StockFactory的this成为shared_ptr
                     boost::noncopyable
{
public:
    shared_ptr<Stock> get(const string &key)
    {
        shared_ptr<Stock> pStock;
        weak_ptr<Stock> &wkStock = stocks_[key]; // 如果map里有该索引，则直接赋值，否则新创建一个该索引
        pStock = wkStock.lock();                 // lock()函数返回一个指向共享对象的shared_ptr，否则返回一个空shared_ptr。
        if (!pStock)                             // 如果为空
        {
            pStock.reset(new Stock(key),
                         boost::bind(&StockFactory::weakDeleteCallBack,
                                     boost::weak_ptr<StockFactory>(shared_from_this()), _1)); // 把shared_from_this转换为weak_ptr,_1等回调时再赋值
            wkStock = pStock;
        }
        return pStock;
    }

private:
    shared_ptr<Stock> weakDeleteCallBack(const boost::weak_ptr<StockFactory> &wkFactory,
                                         Stock *stock)
    {
        boost::shared_ptr<StockFactory> factory(wkFactory.lock());
        if (factory)
        {
            factory->removeStock(stock);
        }
        delete stock;
    }

    void removeStock(Stock *stock)
    {
        if (stock)
        {
            if (mutex != 1)
            {
                mutex = 1;
                stocks_.erase(stock->key);
            }
        }
        mutex = 0;
    }

private:
    mutable int mutex;
    map<string, weak_ptr<Stock>> stocks_;
};

// 测试
void testlonglifefactory()
{
    shared_ptr<StockFactory> factory(new StockFactory);
    {
        shared_ptr<Stock> stock = factory->get("IBM");
        shared_ptr<Stock> stock2 = factory->get("IBM");
        assert(stock == stock2);
        // stock在这里销毁
    }
    // factort在这里销毁
}
void testshortlifefactory()
{
    shared_ptr<Stock> stock;
    {
        shared_ptr<StockFactory> factory(new StockFactory);
        stock = factory->get("IBM");
        shared_ptr<Stock> stock2 = factory->get("IBM");
        assert(stock == stock2);
        // factort在这里销毁
    }
    // stock在这里销毁
}
int main()
{
    testlonglifefactory();
    testshortlifefactory();
}