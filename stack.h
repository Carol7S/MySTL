//
// Created by Carol on 2020/12/29.
//

#ifndef MYSTL_STACK_H
#define MYSTL_STACK_H
#include "deque.h"
#include <iostream>
namespace mystl{
    template<class T, class Container = deque<T>>
    class stack{
    public:
        typedef typename Container::value_type			value_type;
        typedef typename Container::size_type			size_type;
        typedef typename Container::reference			reference;
        typedef typename Container::const_reference		const_reference;
    protected:
        Container   c;   //底层容器
    public:
        stack() : c() {}    //构造函数
        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }
        reference top() { return c.back(); }
        const_reference top() const { return c.back(); }
        //deque是两头可进出，stack是末端进，末端出（LIFO）
        void push(const value_type& x) { c.push_back(x); }
        void pop() { c.pop_back(); }
        friend bool operator==(const stack<T, Container>& x, const stack<T, Container>& y) {
            return (x.cont == y.cont);
        }
        friend bool operator<(const stack<T, Container>& x, const stack<T, Container>& y) {
            return (x.cont < y.cont);
        }

        //stack没有迭代器，只有最上端的元素可以被访问,不提供走访功能，也就没有迭代器
    };
}
#endif //MYSTL_STACK_H
