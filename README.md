# 笔记

### C++
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

2.  ```c++

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

3.  ```c++

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

    回调函数就是一个通过函数指针调用的函数。如果你把函数的指针（地址）作为参数传递给另一个函数，当这个指针被用来调用其所指向的函数时，我们就说这是回调函数。回调函数不是由该函数的实现方直接调用，而是在特定的事件或条件发生时由另外的一方调用的，用于对该事件或条件进行响应。 

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

    这里用到了函数指针（即指向函数的指针），我们要监听A类中的成员变量a，我们定义A类的时候就增加一个将来要监听a变量的函数指针列表，并增加一个registerCallback函数用于将来添加监听者，在a变化时将监听者列表里的所有监听者都调用一遍；在使用A类对象时，我们只要把一个返回类型、参数列表（签名）符合的函数添加为回调函数即可，如上面当我们运行a1.setA(7)改变a的值时，就会调用了回调函数display，这就差不多是事件监听的思想：首先订阅事件（如这里的把display函数注册为回调函数），然后当事件（这里是a的值变化了）发生时，就会自动调用回调函数实现监听。

    ```