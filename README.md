# 笔记

##  注释：

1. `TcpConnection`的`peerAddress()`和`localAddress()`成员函数分别返回对方和本地的地址(以`InetAddress`对象表示IP和port)

2. `TcpConnection::connected()`返回一个`bool`值，表明目前链接是建立还是断开，1为链接，0为断开。

3. 在`onConnection()`中conn参数是`TcpConnection`对象的`shared_ptr`

4. 在`onMassage()`中conn参数是收到数据的那个TCP链接，buf是以及收到的数据，buf的数据会积累，直到用户取走(retrieve)数据。time是`epoll_wait()`的返回时间，这个时间通常比`read()`发生的时间略早。

5. Buffer类通过 `findCRLF` 和`findEOL`成员函数在`readerable`区域检索`\r\n`和`EOL`，返回检索到的地址


## 使用muduo库

### 	1.编译与测试

编译时应链接相应的静态库

```shell
-lmuduo_net
-lmuduo_base
```

检查文件描述符及其状态

```shell
#通过port查找PID
ps -uax | grep port
或
lsof -i:port

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

### 7. filetransfer

```c++
// 设置内容作为成员变量。这个内容可以是任何数据，主要是用着一个临时存储作用。
conn->setContext(); 
//取出该成员变量
conn->getContext();
```

### 8. chat

```c++
muduo::net::Buffer->retrieve(kHeaderLen); // 从头部取走数据，删除取走部分并返回该部分
```

### 9. Protobuf

编译porot文件:

```shell
protoc xxx.proto --cpp_out=./	#编译为 xxx 到 ./ 目录下（c++格式）
```

编译源文件时需链接:

```shell
-lprotobuf
```



接收双方在收到`Protobuf`二进制数据流之后，如何自动创建具体类型的`Protobuf Message`对象，并用收到的数据填充该`Message`对象（反序列化）？（即使`proto`文件更新，也会自动填入新的`Message`类型，不需要更新用户代码）

​	1.用`DescriptorPool::gennerated_pool`找到一个`DescriptorPool`对象，它包含了程序编译的时候所链接的全部`Protobuf Message types`。

​	2.根据`type name`用`DescriptorPool::FindMessageTypeByName()`查找`Descriptor`。

​	3.再用`MessageFactory::generated_factory()`找到`MessageFactory`对象，它能创建程序编译时候所链接的全部`Protobuf Message types`。

​	4.然后用`MessageFactory::GetPrototype()`找到具体`Message type`的`default instance`。

​	5.最后用`prototype->New()`创建对象。

示例如下：

```c++
Message* createMessage(const std::string& typeName){
    Message* message = NULL;
    const Descriptor* descriptor
        = DescroptorPool::generated_pool()->FindMessageTypeByName(typeName);
    if(descriptor){
        const Message* prototype
            = MessageFactory::generated_factory()->GetPrototype(descriptor);
        if(prototype){
            message = prototype->New(); 
        }
    }
    return message;
}
```



调用方式：

```c++
Message* newQuery = createMessage("muduo.Query");	//上述返回需要delete,muduo里不需要
assert(newQuery != NULL);
assert(typeid(*newQuery) == typeid(muduo::Query::default_instance()));
cout << "createMessage(\"muduo.Query\") = " << newQuery << endl;
```

### 10.Discard

`AtomicInt64` ，`AtomicInt32`为原子数据类型，线程安全

### 11. twecho

`circular_buffer` 中文意为环形缓冲区，这是一个固定大小的缓冲区，它被定义成一个环形，当缓冲区满了后，新来的数据会覆盖掉旧的数据。

`unordered_set`的`hash`会去重，所以疯狂`echo`并不会顶掉原有的`bucket`

## muduo 源码



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

2. 
   
    在 c/c++ 中，为了解决一些频繁调用的小函数大量消耗栈空间（栈内存）的问题，特别的引入了 `inline` 修饰符，表示为内联函数。
    
    用模板代替宏。
    ```c++
    函数模板：
    template <typename type> ret-type func-name(parameter list)
    {
        // 函数的主体
    }   
    
    类模板：
    template <class type> class class-name {
    
    }
    实例化类模板：类模板名 <真实类型参数表>
    ```
    
    在这里，`type` 是占位符类型名称，可以在类被实例化的时候进行指定。
    
    

3. 
   
    `mutable`的作用有两点：
    （1）保持常量对象中大部分数据成员仍然是“只读”的情况下，实现对个别数据成员的修改；
    （2）使类的`const`函数可以修改对象的`mutable`数据成员。
    


4. 
   在构建 shared_ptr 智能指针，可以明确其指向。例如：
   
    ```c++
    std::shared_ptr<int> p3(new int(10));
    ```
    由此，我们就成功构建了一个 `shared_ptr` 智能指针，其指向一块存有 10 这个 `int` 类型数据的堆内存空间。
   
    `weak_ptr`是为了配合`shared_ptr`而引入的一种智能指针，它指向一个由`shared_ptr`管理的对象而不影响所指对象的生命周期，也就是将一个`weak_ptr`绑定到一个`shared_ptr`不会改变`shared_ptr`的引用计数。不论是否有`weak_ptr`指向，一旦最后一个指向对象的`shared_ptr`被销毁，对象就会被释放。
   
    当我们创建一个`weak_ptr`时，需要用一个`shared_ptr`实例来初始化`weak_ptr`，由于是弱共享，`weak_ptr`的创建并不会影响`shared_ptr`的引用计数值。
   
    既然`weak_ptr`并不改变其所共享的`shared_ptr`实例的引用计数，那就可能存在`weak_ptr`指向的对象被释放掉这种情况。这时，我们就不能使用`weak_ptr`直接访问对象。那么我们如何判断`weak_ptr`指向对象是否存在呢？C++中提供了`lock`函数来实现该功能。如果对象存在，`lock()`函数返回一个指向共享对象的`shared_ptr`，否则返回一个空`shared_ptr`。
   
    在初始化 `shared_ptr` 智能指针时，还可以自定义所指堆内存的释放规则，这样当堆内存的引用计数为 0 时，会优先调用我们自定义的释放规则。
   
    ```c++
    std::shared_ptr<int> p7(new int[10], deleteInt);
    std::shared_ptr<int> p7(new int[10], [](int* p) {delete[]p; });
    ```
   
5. 
   
    map对象是模板类，需要关键字和存储对象两个模板参数：
    ```c++
    std:map<int, string> personnel;
    ```
    这样就定义了一个用`int`作为索引,并拥有相关联的指向`string`的指针.
    
    `vector`属于顺序容器，其元素与存储位置与操作操作有关；
    `set`属于关联容器，其元素相当于键值。 `set`能够保证它里面所有的元素都是不重复的


6. ```c++
      
    若p为智能指针对象(如：shared_ptr< int> p)
    p.reset(q) //q为智能指针要指向的新对象
    p.reset(); //释放p中内置的指针指向的空间
    p.reset(q.d); //将p中内置指针换为q，并且用d来释放p之前所指的空间
    
    ```

7. 
   ​    
    回调函数就是一个通过函数指针调用的函数。如果你把函数的指针（地址）作为参数传递给另一个函数，当这个指针被用来调用其所指向的函数时，我们就说这是回调函数。
    回调函数不是由该函数的实现方直接调用，而是在特定的事件或条件发生时由另外的一方调用的，用于对该事件或条件进行响应。 
   
    回调函数机制： 
    1、定义一个函数（普通函数即可）； 
    2、将此函数的地址注册给调用者； 
    3、特定的事件或条件发生时，调用者使用函数指针调用回调函数。 
   
    eg:
    ```c++
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
    ```
   
    这里用到了函数指针（即指向函数的指针），我们要监听`A`类中的成员变量`a`，我们定义`A`类的时候就增加一个将来要监听`a`变量的函数指针列表，并增加一个`registerCallback`函数用于将来添加监听者，在`a`变化时将监听者列表里的所有监听者都调用一遍；在使用A类对象时，我们只要把一个返回类型、参数列表（签名）符合的函数添加为回调函数即可，如上面当我们运行`a1`.
   
    `setA(7)`改变`a`的值时，就会调用了回调函数`display`，这就差不多是事件监听的思想：首先订阅事件（如这里的把`display`函数注册为回调函数），然后当事件（这里是`a`的值变化了）发生
    时，就会自动调用回调函数实现监听。


8. 
    在`std::map<string, string>::iterator`中，`it->first`是`key`,`it->second`是`value`


9. 
    `string`中：
    函数`assign()`常用在给`string`类变量赋值.
    函数`erase()`常用在处理删除字符串,返回删除后的字符串
    
10. ```c++
    void call(){
    	::func(); // 外部的全局函数
    	func(); // 成员函数
    }
    ```

11.  类模版`std::function`是一种通用、多态的函数封装。 
     `std::function`的实例可以对任何可以调用的目标实体进行存储、复制、和调用操作，这些目标实体包括普通函数、`Lambda`表达式、函数指针、以及其它函数对象等。


12. 
     `C++ 11`中对`unordered_set`描述大体如下：
     无序集合容器（`unordered_set`）是一个存储唯一(`unique`，即无重复）的关联容器（Associative container），容器中的元素无特别的秩序关系，该容器允许基于值的快速元素检索，同时也支持正向迭代。


​     

## 参考

博客:
<https://www.cnblogs.com/fortunely/>

常见IO模型参考:

<https://blog.csdn.net/u013256816/article/details/115388239>
<https://www.cnblogs.com/yrxing/p/14143644.html>

muduo线程池的one loop per thread:

<https://blog.csdn.net/m0_47891203/article/details/127084649?ops_request_misc=&request_id=&biz_id=102&utm_term=one%20loop%20per%20thread%E6%A8%A1%E5%9E%8B&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduweb~default-2-127084649.142^v68^js_top,201^v4^add_ask,213^v2^t3_esquery_v2>

Protocol Buffers C++:

<https://blog.csdn.net/K346K346/article/details/51754431>
