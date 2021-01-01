//
// Created by Carol on 2020/12/25.
//

#ifndef MYSTL_ITERATOR_H
#define MYSTL_ITERATOR_H
#include <cstddef>
namespace mystl{
    /* 五种迭代器标志 */
    //用来激活函数重载机制
    /* 只写迭代器 */
    struct input_iterator_tag { };
    /* 只读迭代器 */
    struct output_iterator_tag { };
    /* 读写迭代器 */
    struct forward_iterator_tag : public input_iterator_tag { };
    /* 双向迭代器 */
    struct bidirectional_iterator_tag : public forward_iterator_tag { };
    // 随机迭代器  支持++, --, +n, -n, [n], -, <等等，前面的四种最多支持++，--
    struct random_access_iterator_tag : public bidirectional_iterator_tag { };

    /* 为了避免写代码时遗漏某个内嵌型别，自行开发的迭代器最好继承下面这个基类 */
    //必须要传入迭代器型别（以上五种之一）和对象型别
    template<class Category,
            class T,
            class Distance = ptrdiff_t,
            class Pointer = T*,
            class Reference = T&>
    struct iterator {
        typedef Category		iterator_category;      //迭代器的种类，比如 是只能往前，是能跳着走？
        typedef T				value_type;             //it所指向的数据的类型
        typedef Distance		difference_type;        //两个it之间的距离
        typedef Pointer			pointer;                //这两个暂时用不到
        typedef Reference		reference;
    };

    //萃取器traits
    /* iterator_traits负责萃取迭代器的特性 */
    //class point
    template<class Iterator>
    struct iterator_traits {
        typedef typename Iterator::iterator_category	iterator_category;
        typedef typename Iterator::value_type			value_type;
        typedef typename Iterator::difference_type		difference_type;
        typedef typename Iterator::pointer				pointer;
        typedef typename Iterator::reference			reference;
    };

    // 针对原生指针而设计的偏特化版traits
    //no-class point
    template<class T>
    struct iterator_traits<T*> {
        typedef random_access_iterator_tag		iterator_category;
        typedef T								value_type;
        typedef ptrdiff_t						difference_type;
        typedef T*								pointer;
        typedef T&								reference;
    };

    //const point 特化版本
    template<class T>
    struct iterator_traits<const T*> {
        typedef random_access_iterator_tag		iterator_category;
        typedef T								value_type;
        typedef ptrdiff_t						difference_type;
        typedef T*								pointer;
        typedef T&								reference;
    };

    /* 这个函数可以很方便的决定某个迭代器的类型（以上五种迭代器的一个对象） */
    template<class Iterator>
    inline typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator&) {
        typedef typename iterator_traits<Iterator>::iterator_category category;
        return category();
    }

    /* 这个函数可以很方便的决定某个迭代器的distance type */
    template<class Iterator>
    inline typename iterator_traits<Iterator>::difference_type*
    distance_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
        //static_cast相当于传统的C语言里的强制转换
        //static_cast< new_type >(expression)
        //new_type为目标数据类型，expression为原始数据类型变量或者表达式
    }

    /* 这个函数可以很方便的决定某个迭代器的value type */
    template<class Iterator>
    inline typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }

    namespace detail{
            //以下是整组distance函数
        template<typename InputIterator>
        inline typename iterator_traits<InputIterator>::difference_type
        __distance(InputIterator first, InputIterator last, input_iterator_tag){
            typename iterator_traits<InputIterator>::difference_type n = 0;
            while(first != last){
                ++first;
                ++n;
            }
            return n;
        }

        template<typename RandAccessIterator>
        inline typename iterator_traits<RandAccessIterator>::difference_type
        __distance(RandAccessIterator first, RandAccessIterator last, random_access_iterator_tag){
            return last - first;
        }

        /* 以下是整组advance函数细节 */
        template<class InputIterator, class Distance>
        inline void __advance(InputIterator& it, Distance n, input_iterator_tag) {
            while (n--) ++it;
        }

        template<class BidirectionalIterator, class Distance>
        inline void __advance(BidirectionalIterator& it, Distance n, bidirectional_iterator_tag) {
            if (n >= 0)
                while (n--) ++it;
            else
                while (n++) --it;
        }

        template<class RandomAccessIterator, class Distance>
        inline void __advance(RandomAccessIterator& it, Distance n, random_access_iterator_tag) {
            it += n;
        }

        }//detail结束

        // distance和advance外部接口
    template<typename InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last){
        typedef typename
            iterator_traits<InputIterator>::iterator_category category;
        return detail::__distance(first, last, category());
    }

/* advance标准接口 */
    template<class InputIterator, class Distance>
    inline void advance(InputIterator& it, Distance n) {
        detail::__advance(it, n, iterator_category(it));
    }
}
#endif //MYSTL_ITERATOR_H
