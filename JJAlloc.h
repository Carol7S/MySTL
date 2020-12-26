//
// Created by Carol on 2020/12/25.
//

#ifndef MYSTL_JJALLOC_H
#define MYSTL_JJALLOC_H
#include <iostream>   //std::cerr
#include <climits>   //UINT_MAX
#include <cstdlib>  //exit
#include <new>      //placement new
#include <cstddef> //ptrdiff_t, size_t   宏定义   size_t 是unsigned 类型,而 ptrdiff_t 则是signed 整型,和机器相关
//分配器 Allocator
namespace jj
{
    namespace detail
    {
        template<typename T>
        inline T* _allocate(ptrdiff_t size, T*)//分配内存
        {
            std::set_new_handler(0);//设置new失败的异常处理，内存分配失败就直接抛出bad_alloc
            T *tmp = (T*)(::operator new((size_t)(size * sizeof(T))) ); //new的一个指针，再类型转换，
            // (size_t)(size * sizeof(T))是他的参数，进行了类型转换
            if(tmp == nullptr)//如果没有，就退出
            {
                std::cerr << "out of memory" << std::endl;
                exit(1);
            }
            return tmp;
        }

        template <typename T>
        inline void _deallocate(T* ptr)//释放内存
        {
            if(ptr)
                ::operator delete(ptr);
        }

        template <typename T1, typename T2>
        inline void _construct(T1* ptr, const T2& value)//构造
        {
            new(ptr) T1(value);//在已经分配成功的一块内存(ptr所指)上放置对象(value)，自动调用类的构造函数
            // A *a1=new A;
            // A *a2=new(a1) A;//在堆上构造对象
        }

        template <typename T>
        inline void _destroy(T* ptr)//析构
        {
            ptr->~T();  //由于placement new 对象的空间不会自动释放（对象实际上是借用别人的空间），
            // 所以必须显示的调用类的析构函数，如ptr->~T()。
        }
    }
    //简易空间配置器
    template<typename T>
    class allocator
    {
    public:
        typedef T                   value_type;
        typedef T*                  pointer;
        typedef const T*            const_pointer;
        typedef T&                  reference;
        typedef const T&            const_reference;
        typedef size_t              size_type;
        typedef ptrdiff_t           difference_type;

        template<typename U>
        struct rebind
        {
            typedef allocator<U> other;
        };

        pointer allocate(size_type n, const void* hint=0)
        {
            return detail::_allocate((difference_type)n, (pointer)0);
        }

        void deallocate(pointer ptr)
        {
            detail::_deallocate(ptr);
        }

        void construct(pointer ptr, T value)
        {
            detail::_construct(ptr, value);
        }

        void destory(pointer ptr)
        {
            detail::_destroy(ptr);
        }

        pointer address(reference x){return (pointer)&x;}   //返回地址
        const_pointer const_address(const_reference x){return (const_pointer)&x;}
        size_type max_size() const {return (size_type)UINT_MAX/sizeof(T);}   //返回最大个数
    };
}
#endif //MYSTL_JJALLOC_H
