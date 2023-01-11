# 笔记

##  注释：

1. `TcpConnection`的`peerAddress()`和`localAddress()`成员函数分别返回对方和本地的地址(以`InetAddress`对象表示IP和port)

2. `TcpConnection::connected()`返回一个`bool`值，表明目前链接是建立还是断开，1为链接，0为断开。

3. 在`onConnection()`中conn参数是`TcpConnection`对象的`shared_ptr`

4. 在`onMassage()`中conn参数是收到数据的那个TCP链接，buf是以及收到的数据，buf的数据会积累，直到用户取走(retrieve)数据。time是`epoll_wait()`的返回时间，这个时间通常比`read()`发生的时间略早。

5. Buffer类通过 `findCRLF` 和`findEOL`成员函数在`readerable`区域检索`\r\n`和`EOL`，返回检索到的地址


## 使用muduo库

### 	1.编译时应链接相应的静态库

```shell
-lmuduo_net
-lmuduo_base
```

### 2. 服务器编程模式

| 方案  |            名称             | 接受新连接 |      网络IO      |     计算任务     |
| :---: | :-------------------------: | :--------: | :--------------: | :--------------: |
|   2   |    thread-per-connection    |  1个线程   |      N线程       | 在网络线程中进行 |
|   5   |        单线程Reactor        |  1个线程   | 在连接线程中进行 | 在连接线程中进行 |
|   8   |       Reactor+线程池        |  1个线程   | 在连接线程中进行 |      C线程       |
|   9   |     one loop per thread     |  1个线程   |      C线程       | 在网络线程中进行 |
|  11   | one loop per thread +线程池 |  1个线程   |      C线程       |      C线程       |

N表示并发连接数目，C是与连接数无关，与CPU数目有关的常数

###     3. 日志

日志可输出到`LOG_INFO`

### 	4. echo

测试回射服务器echo(单线程):

本地运行，使用`netcat`或`telnet`进行测试：

   ```shell
    nc localhost post #post：xxx
    telnet localhost post #post: xxx
   ```

> `netcat`和`telnet`是一款命令行网络工具，用来在两台机器之间建立TCP/UDP连接，并通过标准的输入输出进行数据的读写

###	5. figner

用muduo实现最简单的7个figner服务端（见muduo/figner/）

Telnet测试：

在一个命令行窗口运行：


```shell
 $ ./finger07.out
```
另一个命令行窗口运行：

```shell
$ telnet localhost 1079
Trying ::1...
Connection failed: 拒绝连接Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
muduo
No such user
Connection closed by foreign host.
```

再试一次：

```shell
$ telnet localhost 1079
Trying ::1...
Connection failed: 拒绝连接Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
schen
Happy and well
Connection closed by foreign host.
```



### 6. sudoku

```c++
muduo::implicit_cast<size_t>(kCells)) // 类型转换<目标类型>（被转换类型）
```



## muduo C++

1. ```c++
   // 基类
   class Animal {
       // eat() 函数
       // sleep() 函数
   };
   
   
   //派生类
   class Dog : public Animal {
       // bark() 函数
   };
   ```

2. ```c++
    
    在 c/c++ 中，为了解决一些频繁调用的小函数大量消耗栈空间（栈内存）的问题，特别的引入了 inline 修饰符，表示为内联函数。
    
    用模板代替宏。
    
    函数模板：
    template <typename type> ret-type func-name(parameter list)
    {
        // 函数的主体
    }   
    
    类模板：
    template <class type> class class-name {
    
    }
    实例化类模板：类模板名 <真实类型参数表>
    
    在这里，type 是占位符类型名称，可以在类被实例化的时候进行指定。
    
    ```

3. ```c++
    
    mutable的作用有两点：
    （1）保持常量对象中大部分数据成员仍然是“只读”的情况下，实现对个别数据成员的修改；
    （2）使类的const函数可以修改对象的mutable数据成员。
    
    ```

4. ```c++
     
    在构建 shared_ptr 智能指针，可以明确其指向。例如：
    std::shared_ptr<int> p3(new int(10));
    由此，我们就成功构建了一个 shared_ptr 智能指针，其指向一块存有 10 这个 int 类型数据的堆内存空间。
    
    weak_ptr是为了配合shared_ptr而引入的一种智能指针，它指向一个由shared_ptr管理的对象而不影响所指对象的生命周期，也就是将一个weak_ptr绑定到一个shared_ptr不会改变shared_ptr的引用计数。不论是否有weak_ptr指向，一旦最后一个指向对象的shared_ptr被销毁，对象就会被释放。
    当我们创建一个weak_ptr时，需要用一个shared_ptr实例来初始化weak_ptr，由于是弱共享，weak_ptr的创建并不会影响shared_ptr的引用计数值。
    
    既然weak_ptr并不改变其所共享的shared_ptr实例的引用计数，那就可能存在weak_ptr指向的对象被释放掉这种情况。这时，我们就不能使用weak_ptr直接访问对象。那么我们如何判断weak_ptr指向对象是否存在呢？C++中提供了lock函数来实现该功能。如果对象存在，lock()函数返回一个指向共享对象的shared_ptr，否则返回一个空shared_ptr。
    
    ```

5. ```c++
    
    map对象是模板类，需要关键字和存储对象两个模板参数：
    std:map<int, string> personnel;
    这样就定义了一个用int作为索引,并拥有相关联的指向string的指针.
    
    ```

6. ```c++
    
    若p为智能指针对象(如：shared_ptr< int> p)
    p.reset(q) //q为智能指针要指向的新对象
    p.reset(); //释放p中内置的指针指向的空间
    p.reset(q.d); //将p中内置指针换为q，并且用d来释放p之前所指的空间
    
    ```

7. ```c++
     
    回调函数就是一个通过函数指针调用的函数。如果你把函数的指针（地址）作为参数传递给另一个函数，当这个指针被用来调用其所指向的函数时，我们就说这是回调函数。
    回调函数不是由该函数的实现方直接调用，而是在特定的事件或条件发生时由另外的一方调用的，用于对该事件或条件进行响应。 
    
    回调函数机制： 
    1、定义一个函数（普通函数即可）； 
    2、将此函数的地址注册给调用者； 
    3、特定的事件或条件发生时，调用者使用函数指针调用回调函数。 
    
    eg:
    
    #include <iostream>
    #include <vector>
    using namespace std;
    class A 
    {
    private:
        int a = 5;
        vector<void(*)(int)> funcs;
    public:
        void setA(int a_);
        void registerCallback(void(*p)(int));
    };
    
    void display(int a) 
    {
        cout << "a=" << a << endl;
    }
    
    int main()
    {
        A a1;
        a1.registerCallback(display);
        a1.setA(7);
        system("pause");
        return 0;
    }
    
    void A::setA(int a_)
    {
        a = a_;
        for (int i = 0; i < funcs.size(); ++i) {
            funcs[i](a);
        }
    }
    
    void A::registerCallback(void(*p)(int))
    {
        funcs.push_back(p);
    }
    
    
    这里用到了函数指针（即指向函数的指针），我们要监听A类中的成员变量a，我们定义A类的时候就增加一个将来要监听a变量的函数指针列表，并增加一个registerCallback函数用于将来
    添加监听者，在a变化时将监听者列表里的所有监听者都调用一遍；在使用A类对象时，我们只要把一个返回类型、参数列表（签名）符合的函数添加为回调函数即可，如上面当我们运行a1.
    setA(7)改变a的值时，就会调用了回调函数display，这就差不多是事件监听的思想：首先订阅事件（如这里的把display函数注册为回调函数），然后当事件（这里是a的值变化了）发生
    时，就会自动调用回调函数实现监听。
    
    ```

8. ```c++
    在std::map<string, string>::iterator中，it->first是key,it->second是value
    ```

9. ```c++
    string中：
    函数assign()常用在给string类变量赋值.
    函数erase()常用在处理删除字符串,返回删除后的字符串
    ```

## 参考

常见IO模型参考:

<https://blog.csdn.net/u013256816/article/details/115388239>
<https://www.cnblogs.com/yrxing/p/14143644.html>

muduo线程池的one loop per thread:

<https://blog.csdn.net/m0_47891203/article/details/127084649?ops_request_misc=&request_id=&biz_id=102&utm_term=one%20loop%20per%20thread%E6%A8%A1%E5%9E%8B&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduweb~default-2-127084649.142^v68^js_top,201^v4^add_ask,213^v2^t3_esquery_v2>