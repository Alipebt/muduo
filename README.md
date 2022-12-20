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
