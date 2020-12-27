//
// Created by Carol on 2020/12/27.
//
//实现for_each, search, sort
//sort未实现堆排序部分以及自定义函数功能
#ifndef MYSTL_ALGO_H
#define MYSTL_ALGO_H
namespace mystl{
    //for_each
    template<class InputIterator, class Function>
    Function for_each(InputIterator first, InputIterator last, Function f){
        for(; first!=last; ++first){
            f(*first);
        }
        return f;
    }
    namespace detail{
        //search 内部
        template<class ForwardIterator1, class ForwardIterator2, class Distance1, class Distanec2>
        ForwardIterator1 __search(ForwardIterator1 first1, ForwardIterator1 last1,
                                  ForwardIterator2 first2, ForwardIterator2 last2,
                                  Distance1*, Distanec2*)
        {
            Distance1 d1 = distance(first1, last1, d1);
            Distanec2 d2 = distance(first2, last2, d2);
            if(d1 < d2) return last1;  //如果第二序列大于第一序列，那不可能成为他的子序列
            ForwardIterator1 current1 = first1;
            ForwardIterator2 current2 = first2;
            while(current2 != last2){
                if(*current1 == *current2){
                    ++current1;
                    ++current2;
                } else{//如果元素不相等
                    if(d1 == d2)//如果长度相同，那就不可能是子序列了
                        return last1;
                    else{//序列1的长度大于序列2
                        current1 = ++first1;     //第一序列的标兵向后一位
                        current2 = first2;      //重新调整序列2的起点
                        --d1;                   //已经排除序列1的第一个元素，所以要--
                    }
                }
            }
            return first1;
        }
    }// end of namespace detail
    //search外部接口
    template<class ForwardIterator1, class ForwardIterator2>
    inline ForwardIterator1 search(ForwardIterator1 first1,
                                   ForwardIterator1 last1,
                                   ForwardIterator2 first2,
                                   ForwardIterator2 last2)
    {
        return __search(first1, last1, first2, last2, distance_type(first1), distance_type(first2));
    }


    //从这里开始是sort
    //避免过长，去掉了heapsort的部分  XD(摸了)
    const int __stl_threshold = 16;
    namespace detail{
        /*
        template<class RandomAccessInterator>
        inline void __partial_sort(RandomAccessInterator first,
                                    RandomAccessInterator middle,
                                    RandomAccessInterator last,
                                    T*){
            make_heap(first, middle);
        }
        template<class RandomAccessInterator>
        inline void partial_sort(RandomAccessInterator first,
                                RandomAccessInterator middle,
                                RandomAccessInterator last){
            __partial_sort(first, middle ,last, value_type(first));
        }
        */

        template<class RandomAccessInterator, class T>
        void __unguarded_linear_insert(RandomAccessInterator last, T value){
            RandomAccessInterator next = last;
            --next;
            //insertion sort的内循环
            //注意，不再出现逆转对，就可以结束循环
            while(value < *next){//逆转对存在
                *last = *next; //调整
                last = next;//调整逆转对
                --next;//左移一个位置
            }
            *last = value;//value的正确位置
        }

        template<class RandomAccessInterator, class T>
        inline void __linear_insert(RandomAccessInterator first,
                                    RandomAccessInterator last, T*){
            T value = *last;//记录尾元素
            if(value < *first){//如果尾比头还小
                copy_backward(first, last, last+1);  //整个区间向右移动一个位置
                *first = value;                     //头元素等于之前的尾元素
                //例如 5,4,3,2,1变成 1,5,4,3,2
            } else{
                __unguarded_linear_insert(last, value);
            }

        }

        template<class RandomAccessInterator>
        void __insertion_sort(RandomAccessInterator first,
                              RandomAccessInterator last){
            if(first == last) return;
            for(RandomAccessInterator i = first+1; i != last; ++i)//外循环
                __linear_insert(first, i, value_type(first));
            //以上 [first,i)形成一个子区间
        }

        //三点中值决定函数
        template<class T>
        inline const T& __median(const T& a, const T& b, const T& c){
            if(a < b)
                if(b < c)  //a < b < c
                    return b;
                else if(a < c)
                    return c;   //c在中间
                else
                    return a;   //a在中间
            else if (a < c)
                return a;
            else if (b < c)
                return c;
            else
                return b;
        }

        template<typename _ForwardIterator1, typename _ForwardIterator2>
        inline void
        iter_swap(_ForwardIterator1 __a, _ForwardIterator2 __b)
        {
            typedef typename iterator_traits<_ForwardIterator1>::value_type
                    _ValueType1;
            _ValueType1 __tmp = *__a;
            *__a = *__b;
            *__b = __tmp;
        }


        template<class RandomAccessInterator, class T>
        RandomAccessInterator __unguarded_partition(RandomAccessInterator first,
                                                    RandomAccessInterator last,
                                                    T pivot){
            while(true){
                while(*first < pivot) ++first;      //first找到 >= pivot 就停下
                --last;                             //调整
                while(pivot < *last) --last;         //last 找到 <=pivot 就停下
                if(!(first < last)) return first;    //交错，相遇，结束循环
                iter_swap(first, last);             //大小值交换
                ++first;                            //调整
            }
        }

        template<class Size>
        inline Size __lg(Size n){
            //用来控制分割恶化的情况
            //找出2^k < n 的最大的k
            Size k;
            for(k=0; n>1; n>>=1) ++k; //>>=1   等于 /2
            return k;
        }

        template<class RandomAccessIterator, class T, class Size>
        void __introsort_loop(RandomAccessIterator first,
                              RandomAccessIterator last,
                              T*,
                              Size depth_limit)
        {
            while(first - last >= __stl_threshold){
                /*
                if(depth_limit == 0){//至此，分割恶化
                    partial_sort(first, last, last);//改用heapsort
                    return;
                }
                 */
                --depth_limit;
                //以下是median of-3 partition，选择一个够好的枢轴并决定分割点
                //分割点将落在迭代器cut上
                RandomAccessIterator cut = __unguarded_partition
                        (first, last, T(__median(*first,
                                                 *(first + (last - first)/2),
                                                 *(last-1))));
                //对右半段进行sort
                __introsort_loop(cut, last, value_type(first), depth_limit);
                last = cut;
                //现在准备回到while循环，对左半段递归进行sort
            }
        }

        template<class RandomAccessIterator, class T>
        void __unguarded_insertion_insert_aux(RandomAccessIterator first,
                                              RandomAccessIterator last,
                                              T*){
            for(RandomAccessIterator i=first; i != last; ++i){
                __unguarded_linear_insert(i, T(*i));
            }
        }

        template<class RandomAccessIterator>
        void __unguarded_insertion_insert(RandomAccessIterator first,
                                          RandomAccessIterator last){
            __unguarded_insertion_insert_aux(first, last);
        }

        template<class RandomAccessIterator>
        void __final_insertion_sort(RandomAccessIterator first,
                                    RandomAccessIterator last){
            if(last-first > __stl_threshold){//>16
                __insertion_sort(first, first+__stl_threshold);
                __unguarded_insertion_insert(first+__stl_threshold, last);
            } else{
                __insertion_sort(first, last);
            }
        }
    } // end of namespace detail
    //sort外部接口
    //注意，sort只适用于RandomAccessIterator
    template<class RandomAccessIterator>
    inline void sort(RandomAccessIterator first,
                     RandomAccessIterator last)
    {
        if(first != last){
            detail::__introsort_loop(first, last, value_type(first), detail::__lg(last-first)*2);
            detail::__final_insertion_sort(first, last);
        }
    }
}
#endif //MYSTL_ALGO_H
