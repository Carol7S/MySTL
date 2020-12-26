//
// Created by Carol on 2020/12/25.
//

#ifndef MYSTL_ALLOCTOR_H
#define MYSTL_ALLOCTOR_H
#include <stdlib.h>  //malloc realloc free

#if 0
        #include <new>
        #define __THROW_BAD_ALLOC throw std::exception("bad_alloc");
#elif !defined(__THROW_BAD_ALLOC)
        #include <iostream>
        #define __THROW_BAD_ALLOC std::cerr << "out of memory!" << std::endl; exit(1);
#endif

namespace mystl     //第一级namespace是提供给外部用户使用的（后同）
{
    //第一级配置 malloc_alloc，第二级配置器 __default_alloc_template。
    namespace detail  //第二级namespace是stl内部使用的实现细节（后同）
    {
        //第一级配置器 __malloc_alloc_template
        //以malloc() realloc() free() 等C函数执行实际的内存配置与释放操作
        /* 无template型别参数，非型别参数inst也并没有使用 */
        template<int inst>
        class __malloc_alloc_template
        {
        private:
            /* 以下函数将用来处理内存不足的情况 */
            static void* oom_malloc(size_t);
            static void* oom_realloc(void*, size_t);
            static void(*__malloc_alloc_oom_handler)();

        public:
            static void* allocate(size_t n)
            {
                void *result = ::malloc(n);
                if(result == 0) oom_malloc(n);
                return result;
            }

            static void deallocate(void* p, size_t n)
            {
                ::free(p);
            }

            static void* reallocate(void *p, size_t old_sz, size_t new_sz)
            {
                void *result = ::realloc(p, new_sz);
                if(result == nullptr)
                {
                    result = oom_realloc(p, new_sz);
                }
                return result;
            }

            //以下仿真C++的set_new_handler()，可以通过它指定你自己的out-of-memory-handler
            /* 传入一个函数指针，返回一个函数指针 */
            static void(*set_malloc_handler(void(*f)() ))()
            {
                void(*old)() = __malloc_alloc_oom_handler;
                __malloc_alloc_oom_handler = f;
                return old;
            }
        };


        //malloc_alloc out of memory handing,异常处理
        //初值为0
        template<int inst>
        void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

        //开始定义函数  3个内存不足分配函数
        template<int inst>
        void* __malloc_alloc_template<inst>::oom_malloc(size_t n)
        {
            void(*my_malloc_handler) ();
            void *result;

            for(;;)  //不断尝试释放，分配
            {
                my_malloc_handler = __malloc_alloc_oom_handler;
                if(my_malloc_handler == 0){__THROW_BAD_ALLOC;} //未定义处理例程就退出
                (*my_malloc_handler)();  //调用处理例程，企图释放内存
                result = ::malloc(n);     //尝试分配内存
                if(result) return result;
            }
            __THROW_BAD_ALLOC;
        }

        template<int inst>
        void *__malloc_alloc_template<inst>::oom_realloc(void * p, size_t n)
        {
            void(*my_malloc_handler)();
            void *result;
            for(;;)
            {
                my_malloc_handler = __malloc_alloc_oom_handler;
                if(my_malloc_handler == 0){__THROW_BAD_ALLOC;}
                (*my_malloc_handler)();
                result = ::realloc(p, n);
                if(result) return result;
            }
            __THROW_BAD_ALLOC;
        }

        typedef __malloc_alloc_template<0> malloc_alloc;  //重命名


        enum{ __ALIGN = 8 };                          // 小型区块的上调边界, 对齐
        enum{ __MAX_BYTES = 128};                     //小型区块的上限
        enum{ __NFREELISTS = __MAX_BYTES / __ALIGN}; //自由链表的个数 16

        //第二级配置器 __default_alloc_template
        /* 无template型别参数，第一个参数用于多线程环境，非型别参数inst并没有使用 */
        //第二级配置器，区块超过128bytes，就交给第一级配置器，否则交给内存池，内存池由16条链表组成，自动对齐8
        template<bool threads, int inst>
        class __default_alloc_template{
        private:
            //ROUND_UP() 上调至8的倍数
            static size_t ROUND_UP(size_t bytes)
            {
                return (((bytes) + __ALIGN-1) & ~(__ALIGN-1));
            }
        private:
            union obj       //free_lists的节点构造
            {
                union obj *free_list_link;
                char client_data[1];
            };
        private:
            static obj * volatile free_list[__NFREELISTS];
            //遇到这个volatile关键字声明的变量，编译器对访问该变量的代码就不再进行优化
            //以下函数根据区块的大小，决定使用第n号free_lists,n从1算起
            static size_t FREELIST_INDEX(size_t bytes)
            {
                return (((bytes) + __ALIGN-1)/__ALIGN-1);
            }
            //返回一个大小为n的区块，可能导致区块大小为n的free-list被填充
            static void* refill(size_t n);
            //配置一大块空间，可容纳nobjs个大小为size的区块
            //若配置 nobjs 个区块不便，nobjs可能会降低
            static char *chunk_alloc(size_t size, int &nobjs);

            //chunk allocation state
            static char *start_free; //内存池起始位置，只在chunk_alloc()中变化
            static char *end_free;  //内存池结束位置，只在chunk_alloc()中变化
            static size_t heap_size;
        public:
            /* 空间配置 */
            static void* allocate(size_t n);//详述于后
            /* 空间释放 */
            static void deallocte(void*, size_t );//详述于后
            /* 重新配置空间大小 */
            static void* reallocate(void*, size_t, size_t);//详述于后
        };

        /* static data member 的初值设定 */
        template<bool threads, int inst>
        char* __default_alloc_template<threads, inst>::start_free = 0;
        template<bool threads, int inst>
        char* __default_alloc_template<threads, inst>::end_free = 0;
        template<bool threads, int inst>
        size_t __default_alloc_template<threads, inst>::heap_size = 0;

        template<bool threads, int inst>
        //typename为了告诉编译器，这不是个变量，而是个类型，所以就加上typename
        typename __default_alloc_template<threads, inst>::obj*
        volatile __default_alloc_template<threads, inst>::free_list[__NFREELISTS] =
                { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

        //空间配置函数allocate()，此函数首先判断区块大小，来使用1级配置器or内存池（2级）
        template<bool threads, int inst>
        //n must be > 0
        void* __default_alloc_template<threads, inst>::allocate(size_t n)
        {

            obj* volatile * my_free_list;  //指针的指针
            obj* result;
            //大于128bytes，调用第一级配置器
            if(n > (size_t)__MAX_BYTES)
            {
                return (malloc_alloc::allocate(n));
            }
            //否则寻找16个free list中最适当的一个
            /* 因为free_lists是一个指针数组，同时数组又可以转化为指针使用，所有free_lists相当于指向指针的指针 */
            my_free_list = free_list + FREELIST_INDEX(n);
            result = *my_free_list;
            if(result == 0)
            {
                //没有找到可用的free_list，准备重新填充free_list
                void* r = refill(ROUND_UP(n));
                return r;
            }
            //调整free_list
            *my_free_list = result->free_list_link;
            return result;
        }

        template<bool threads, int inst>
        /* 空间释放函数, p不能为0 */
        void __default_alloc_template<threads, inst>::deallocte(void *p, size_t n)
        {
            obj* q = (obj *) p;
            obj* volatile * my_free_list;

            //大于128bytes就调用第一级
            if(n > (size_t)__MAX_BYTES)
            {
                malloc_alloc::deallocate(p, n);
                return;
            }
            //寻找对应的free_list
            my_free_list = free_list + FREELIST_INDEX(n);
            //调整free_list, 回收区块
            q->free_list_link = *my_free_list;
            *my_free_list = q;
        }

        template<bool threads, int inst>
        //realloc
        void* __default_alloc_template<threads, inst>::reallocate(void *p, size_t old_sz, size_t new_sz)
        {
            //直接调用第一级
            return malloc_alloc ::reallocate(p, old_sz, new_sz);
        }

        /* 重新填充free list，返回一个大小为 size 的内存块给客端使用，其余纳入自由链表 */
        //取得20个新区块
        /* size 已经增加至 8 的倍数，如果得到不止一个内存块则将其他内存块链接到 free list 上 */
        template<bool threads, int inst>
        void* __default_alloc_template<threads, inst>::refill(size_t n)
        {
            int nobjs = 20;
            //调用chunk_alloc()尝试获得 nobjs 个区块
            /* 注意nobjs是引用 */
            char* chunk = chunk_alloc(n, nobjs);
            obj* volatile * my_free_list;
            obj* result;
            obj *current_obj, *next_obj;
            int i;
            //如果只获得一个区块，就直接返回给调用者用,free_list无新节点
            if(nobjs == 1) return chunk;
            //否则调整free_list,准备迎接新节点
            my_free_list = free_list + FREELIST_INDEX(n);
            //以下在chunk中建立free_list
            result = (obj*) chunk;
            //以下引导free_list指向新配置的空间
            *my_free_list = next_obj = (obj*) (chunk+n);
            //以下将free_list各节点串起来
            for(i=1; ; i++)//从1开始，因为第0个返回给客端
            {
                current_obj = next_obj;
                next_obj = (obj*)((char*)next_obj+n);
                if(nobjs - 1 == i)
                {
                    current_obj->free_list_link = 0;
                    break;
                } else
                {
                    current_obj->free_list_link = next_obj;
                }
            }
            return result;
        }

        //内存池
        template<bool threads, int inst>
        char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int &nobjs)
        {
            char *result;
            size_t total_bytes = size * nobjs;
            size_t bytes_left = end_free - start_free; //内存池剩余空间

            if(bytes_left >= total_bytes)//剩余空间足够
            {
                result = start_free;
                start_free += total_bytes;
                return result;
            }
            else if (bytes_left >= size)//内存池还能提供至少一个区块
            {
                nobjs = bytes_left/size;
                total_bytes = nobjs*size;
                result = start_free;
                start_free += total_bytes;
                return result;
            }else{
                //连一个都无法提供
                size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
                //以下试着让内存池中的残余空间还有利用价值
                if(bytes_left > 0)
                {
                    //内存池中还有一些零头，试着配给合适的free_list
                    //首先寻找适当的free_list
                    obj* volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
                    //调整free_list,将内存池的残余空间编入
                    ((obj*)start_free)->free_list_link = *my_free_list;//头部，将这一块加入到头部，然后头部上移
                    *my_free_list = (obj*)start_free;
                }

                //配置heap空间，补充内存池
                start_free = (char*)malloc(bytes_to_get);
                if(0 == start_free)//配置失败
                {
                    int i;
                    obj* volatile * my_free_list, *p;
                    //以下搜索适当的free list，即“尚有未用区块，且区块足够大”的free list
                    for(i = size; i <= __MAX_BYTES; i += __ALIGN)
                    {
                        my_free_list = free_list + FREELIST_INDEX(i);
                        p = *my_free_list;
                        if(0 != p)//尚有未用的空间
                        {
                            //调整free_list以释出未用区块
                            *my_free_list = p->free_list_link;
                            start_free = (char*) p;
                            end_free = start_free + i;
                            //递归调用自己，修正nobjs
                            return (chunk_alloc(size, nobjs));
                        }
                    }
                    end_free = 0; //出现意外，到处都没有内存了，试着调用第一级，看看能不能来点内存
                    start_free = (char*)malloc_alloc::allocate(bytes_to_get);
                    //这会抛出异常 或 获取内存
                }
                heap_size += bytes_to_get;
                end_free = start_free + bytes_to_get;
                //递归调用自己，修正nobjs
                return (chunk_alloc(size, nobjs));
            }
        }

        #define __NODE_ALLOCATER_THREADS false	 //非多线程环境
    } //end of namespace detail

#ifdef __USE_MALLOC		//使用一级空间配置器
    typedef detail::malloc_alloc alloc;
#else					//使用二级空间配置器
    typedef detail::__default_alloc_template<__NODE_ALLOCATER_THREADS, 0> alloc;  //非多线程环境，直接将inst指定为0
#endif

    //SGI封装的标准的alloc配置器接口（一般都使用这个标准配置器接口）
    //使用这个接口时配置大小不再是以字节为单位，而是以元素大小为单位
    template<typename T, class Alloc>
    class simple_alloc{
    public:
        static T* allocate(size_t n){
            return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
        }
        static T* allocate() {
            return (T*)Alloc::allocate(sizeof(T));
        }
        static void deallocate(T* p, size_t n) {
            if (0 != n)
                Alloc::deallocate(p, n * sizeof(T));
        }
        static void deallocate(T* p) {
            Alloc::deallocate(p, sizeof(T));
        }
    };

} // end of namespace mystl
#endif //MYSTL_ALLOCTOR_H
