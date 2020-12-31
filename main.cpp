//#include "alloctor.h"
//#include "iterator.h"
//#include "__type_traits.h"
//#include "construct.h"
//#include "algobase.h"
//#include "algorithm.h"
//#include "uninitialized.h"
#include <iostream>
#include "vector.h"
#include "list.h"
#include "deque.h"
#include "stack.h"
#include "queue.h"
#include "rbtree.h"
#include "multimap.h"
//using namespace std;
using namespace mystl;
void vector_test(){
    std::cout << "Start test of vector" << std::endl;
    vector<int> Ivec;

    mystl::vector<double> Dvec;
    Ivec.push_back(2);
    Ivec.push_back(4);
    std::cout << Ivec[0] << std::endl;
    Dvec.push_back(2.13);
    std::cout << Dvec[0] << std::endl;

};

void list_test(){
    std::cout << "Start test of list" << std::endl;
    list<int> Ilist;
    list<double> Dlist;
    Ilist.push_back(2);
    Ilist.push_front(3);
    list<int>::iterator it;
    it = Ilist.begin();
    ++it;
    Ilist.insert(it, 5);
    Dlist.push_back(3.14);
    for (const auto &item : Ilist) {
        std::cout << item << std::endl;
    }
}

int main() {
    int ch;
    std::cout << "**********************************" << std::endl;
    std::cout << "   **** 请选择要进行的测试: ***"  << std::endl;
    std::cout << "   ******   1. vector    ******" << std::endl;
    std::cout << "   ******   2. list      ******" << std::endl;
    std::cout << "   ******   3. deque     ******" << std::endl;
    std::cout << "   ******   4. stack     ******" << std::endl;
    std::cout << "   ******   5. multimap  ******" << std::endl;
    std::cout << "**********************************" << std::endl;
    std::cout << "Your Choice: ";
    std::cin >> ch;

    switch (ch) {
        case 1:
            vector_test();
            break;
        case 2:
            list_test();
            break;
        default:
            std::cout << "Wrong!" << std::endl;
    }

    return 0;
}
