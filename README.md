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

    ```
5. ```
    map对象是模板类，需要关键字和存储对象两个模板参数：
    std:map<int, string> personnel;
    这样就定义了一个用int作为索引,并拥有相关联的指向string的指针.
    ```