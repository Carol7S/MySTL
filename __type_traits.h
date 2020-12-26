//
// Created by Carol on 2020/12/26.
//
//这是SGI独有的，不在STL标准内
//iterator_traits负责萃取迭代器的特性，__type_traits负责萃取型别(T)的特性

//non-trivial   非平凡的
//平凡的就可以不用管构造之类的，如果非平凡，就要进行处理
/*此处我们所关注的型别特性是指:这个型别是否具备non-trivial defalt ctor?
 * 是否具备non-trivial copy ctor?是否具备non-trivial assignment operator?是否具备non-trivial dtor?
 * 如果答案是否定的，我们在对这个型别进行构造、析构、拷贝、赋值等操作时，就可以采用最有效率的措施
 * （例如根本不调用身居高位，不谋实事的那些constructor,destructor)，而采用内存直接处理操作如malloc() 、
 * memcpy()等等，获得最高效率。这对于大规模而操作频繁的容器，有着显著的效率提升。*/

//个人理解，为了效率，看这个传进来的T是平凡的还是不平凡的，来决定相应的措施
#ifndef MYSTL___TYPE_TRAITS_H
#define MYSTL___TYPE_TRAITS_H
namespace mystl{
    namespace detail{
        /*根据iterator_traits的经验我们希望这样使用__type_traits<T>, T表示型别
        __type_traits<T>::has_trivial_default_constructor
        __type_traits<T>::has_trivial_copy_constructor
         __type_traits<T>::has_trivial_assignment_constructor
         __type_traits<T>::has_trivial_destructor
        __type_traits<T>::is_POD_type    //plain old data

	    他们返回“真”或“假”，以便我们决定采取什么策略，但bool值不能进行函数的重载与决议，
	    因此我们定义以下两个具有真假性质的类，通过对象来进行函数的重载与决议*/

        struct __true_type { };  //可以使用如memcpy()，copy()等快速函数，对应平凡的，trivial
        struct __false_type { }; //不使用，对应不平凡的，no-trivial
        //这两个空白class没有任何成员，不会带来任何负担

        /* 为了达成上述式子，__type_traits内必须定义一些typedefs，其值不是__true_type就是__false_type */
        template<typename type>
        struct __type_traits{
            //通知编译器，现在使用的__type_traits template 是特殊的,不要移除，防止同名
            typedef __true_type         this_dummy_member__must_be_first;

            typedef __false_type	has_trivial_default_constructor;    //默认构造函数
            typedef __false_type	has_trivial_copy_constructor;       //拷贝构造函数
            typedef __false_type	has_trivial_assignment_operator;    //运算符重载
            typedef __false_type	has_trivial_destructor;             //析构
            typedef __false_type	is_POD_type;
            //SGI把所有内嵌型别都定义为__false_type，这是最保守的值（调用其ctor、detor），//构造，析构
            //POD用来描述一个类型的属性其中
            // Plain表示这个类型是个平凡的类型，Old表示其与C的兼容性。
            // C++11中将POD划分为两个基本概念：平凡的（trival）和标准布局（standardlayout）。

            //平凡性： 大意就是不能自己写构造函数之类的，要用编译器自动生成的
            /*具有平凡的默认构造函数。如果我们不自己为类定义任何构造函数，编译器就会为我们产生一个平凡的默认构造函数；
             * 一旦我们为类定义了任何一种构造函数，那这个构造函数就不是平凡的。*/
        };

        /* 以下是对所有c++标量型别所定义的__type_traits特化版本。
	 *这些定义对于内建有__type_traits支持能力的编译器并无伤害，对于无该等支持能力的编译器则属于必要。
	 *
	 *以下为C++基本型别提供特化版本，每一个成员值都是__true_type，
	 *表示这些型别都可以采用最快速方式（例如memcpy）进行拷贝、赋值等操作。
	 */
        template<> struct __type_traits<char> { //特化
            typedef __true_type		has_trivial_default_constructor;
            typedef __true_type		has_trivial_copy_constructor;
            typedef __true_type		has_trivial_assignment_operator;
            typedef __true_type		has_trivial_destructor;
            typedef __true_type		is_POD_type;
        };

        template<> struct __type_traits<signed char> {
            typedef __true_type		has_trivial_default_constructor;
            typedef __true_type		has_trivial_copy_constructor;
            typedef __true_type		has_trivial_assignment_operator;
            typedef __true_type		has_trivial_destructor;
            typedef __true_type		is_POD_type;
        };

        template<> struct __type_traits<unsigned char> {
            typedef __true_type		has_trivial_default_constructor;
            typedef __true_type		has_trivial_copy_constructor;
            typedef __true_type		has_trivial_assignment_operator;
            typedef __true_type		has_trivial_destructor;
            typedef __true_type		is_POD_type;
        };

        template<> struct __type_traits<short> {
            typedef __true_type		has_trivial_default_constructor;
            typedef __true_type		has_trivial_copy_constructor;
            typedef __true_type		has_trivial_assignment_operator;
            typedef __true_type		has_trivial_destructor;
            typedef __true_type		is_POD_type;
        };

        template<> struct __type_traits<unsigned short> {
            typedef __true_type		has_trivial_default_constructor;
            typedef __true_type		has_trivial_copy_constructor;
            typedef __true_type		has_trivial_assignment_operator;
            typedef __true_type		has_trivial_destructor;
            typedef __true_type		is_POD_type;
        };

        template<> struct __type_traits<int> {
            typedef __true_type		has_trivial_default_constructor;
            typedef __true_type		has_trivial_copy_constructor;
            typedef __true_type		has_trivial_assignment_operator;
            typedef __true_type		has_trivial_destructor;
            typedef __true_type		is_POD_type;
        };

        template<> struct __type_traits<unsigned int> {
            typedef __true_type		has_trivial_default_constructor;
            typedef __true_type		has_trivial_copy_constructor;
            typedef __true_type		has_trivial_assignment_operator;
            typedef __true_type		has_trivial_destructor;
            typedef __true_type		is_POD_type;
        };

        template<> struct __type_traits<long> {
            typedef __true_type		has_trivial_default_constructor;
            typedef __true_type		has_trivial_copy_constructor;
            typedef __true_type		has_trivial_assignment_operator;
            typedef __true_type		has_trivial_destructor;
            typedef __true_type		is_POD_type;
        };

        template<> struct __type_traits<unsigned long> {
            typedef __true_type		has_trivial_default_constructor;
            typedef __true_type		has_trivial_copy_constructor;
            typedef __true_type		has_trivial_assignment_operator;
            typedef __true_type		has_trivial_destructor;
            typedef __true_type		is_POD_type;
        };

        template<> struct __type_traits<float> {
            typedef __true_type		has_trivial_default_constructor;
            typedef __true_type		has_trivial_copy_constructor;
            typedef __true_type		has_trivial_assignment_operator;
            typedef __true_type		has_trivial_destructor;
            typedef __true_type		is_POD_type;
        };

        template<> struct __type_traits<double> {
            typedef __true_type		has_trivial_default_constructor;
            typedef __true_type		has_trivial_copy_constructor;
            typedef __true_type		has_trivial_assignment_operator;
            typedef __true_type		has_trivial_destructor;
            typedef __true_type		is_POD_type;
        };

        template<> struct __type_traits<long double> {
            typedef __true_type		has_trivial_default_constructor;
            typedef __true_type		has_trivial_copy_constructor;
            typedef __true_type		has_trivial_assignment_operator;
            typedef __true_type		has_trivial_destructor;
            typedef __true_type		is_POD_type;
        };

        //以下是针对原生指针设计的偏特化版本，原生指针也被视为一种标量型别
        template<class T>
        struct __type_traits<T*> {
            typedef __true_type		has_trivial_default_constructor;
            typedef __true_type		has_trivial_copy_constructor;
            typedef __true_type		has_trivial_assignment_operator;
            typedef __true_type		has_trivial_destructor;
            typedef __true_type		is_POD_type;
        };
    } //end of detail
} //end of mystl

#endif //MYSTL___TYPE_TRAITS_H
