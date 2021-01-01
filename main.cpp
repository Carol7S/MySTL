#include <iostream>
#include <string>
#include "vector.h"
#include "list.h"
#include "deque.h"
#include "stack.h"
#include "rbtree.h"
#include "multimap.h"
#include "algorithm.h"
using namespace mystl;
using namespace std;
class Test{
public:
    Test(){
    }
};

void vector_test(){
    std::cout << "Start test of vector" << std::endl;
    vector<int> Ivec;

    mystl::vector<double> Dvec;
    Ivec.push_back(2);
    Ivec.push_back(4);
    std::cout << "Ivec.size() = " << Ivec.size() << " Ivec.capacity() = " << Ivec.capacity() << std::endl;
    std::cout << Ivec[0] << std::endl;
    Dvec.push_back(2.13);
    std::cout << Dvec[0] << std::endl;

    vector<Test> vec1;
    vector<Test> vec2(vec1);
    vector<int> vec3 = {2, 4, 6};
    vector<Test> vec4(10);
    vector<int> vec5 = {2, 4};

    vec3.push_back(1);
    vector<int>::iterator it1 = find(vec3.begin(), vec3.end(), 2);	//找到1
    cout << *it1 << endl;
    cout << vec3[1] << endl;

    sort(vec3.begin(), vec3.end());
    for (const auto &item : vec3) {
        cout << item << " ";
    }
    cout << endl;
    sort(vec5.begin(), vec5.end());

    vector<int>::iterator it_s;
    it_s = search(vec3.begin(), vec3.end(), vec5.begin(), vec5.end());
    cout << *it_s << endl;

    vec3.insert(vec3.end(), 10);
    vec3.push_front(20);
    vec3.pop_front();
    vec3.pop_back();
    vec3.insert(vec3.begin(), 30);
    vec3.erase(vec3.begin());
    for (const auto &item1 : vec3) {
        cout << item1 << " ";
    }
    cout << endl;
}

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

    list<int> list1;
    list<Test> list2;
    list<int> list3;

    list1.push_back(10);
    list1.push_front(20);
    cout << "size: " << list1.size() << endl;
    cout << "front: " << list1.front() << " back: " << list1.back() << endl;
    list1.insert(list1.begin(), 30);
    for (const auto &list11 : list1) {
        cout << list11 << " ";
    }
    cout << endl;

    list1.sort();
    for (const auto &list13 : list1) {
        cout << list13 << " ";
    }
    cout << endl;

    list1.reverse();
    for (const auto &list12 : list1) {
        cout << list12 << " ";
    }
    cout << endl;

    list1.remove(30);
    for (const auto &list14 : list1) {
        cout << list14 << " ";
    }
    cout << endl;
    list1.clear();

}

void deque_test(){
    std::cout << "start test of deque" << std::endl;
    deque<int> ideq;

    ideq.push_back(2);
    std::cout << ideq[0] << std::endl;
    std::cout << ideq.size() << std::endl;

    deque<double> deq1;
    deque<double> deq2;
    deque<Test> deq3;
    deq1.push_back(10);
    deq1.push_front(20);
    deq1.insert(deq1.begin(), 30);
    cout << "size: " << deq1.size() << " max_size: " << deq1.max_size() << endl;
    cout << "front: " << deq1.front() <<" back: " << deq1.back() << endl;
    sort(deq1.begin(), deq1.end());
    for (const auto &item : deq1) {
        cout << item << " ";
    }
    cout << endl;
    deq1.clear();
}

void stack_test(){
    std::cout << "start test of stack" << std::endl;
    stack<int> istack;
    istack.push(2);
    istack.push(3);
    std::cout << istack.top() << std::endl;
    std::cout << istack.size() << std::endl;
    stack<double> stack1;
    stack1.push(3.14);
    stack1.push(2.32);
    cout << "size: " << stack1.size() << " empty: " << stack1.empty() << endl;
    stack1.pop();
    cout << stack1.top() << endl;
}



void multimap_test(){
    std::cout << "start test of multimap" << std::endl;

    multimap<int, std::string> map1;
    map1[1] = "apple";
    map1[2] = "banner";
    map1[3] = "cat";
    map1[3] = "dog";
    map1[3] = "air";
    multimap<int, std::string>::iterator it;
    std::cout << map1.size() << std::endl;
    for(it = map1.begin(); it != map1.end(); ++it){
        std::cout << it->first << "  " << it->second << std::endl;
    }
    cout << endl;
    map1.insert(pair<int, string>(4, "car"));
    map1.insert(make_pair(5, "hot"));
    for(it = map1.begin(); it != map1.end(); ++it){
        cout << it->first << " " << it->second << endl;
    }

    cout << "count: " << map1.count(3) << endl;
    it = map1.find(3);
    cout << "find: " << it->second << endl;
    ++it;
    cout << "2: " << it->second << endl;
    ++it;
    cout << "3: " << it->second << endl;
    ++it;
    cout << "4: " << it->second << endl;
    pair<multimap<int, string>::iterator , multimap<int, string>::iterator> ret;
    ret = map1.equal_range(3);
    for(it = ret.first; it != ret.second; ++it){
        cout << it->first <<" " << it->second << endl;
    }

    it = map1.find(2);
    cout << "find2: " << it->second << endl;

    map1.clear();
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
        case 3:
            deque_test();
            break;
        case 4:
            stack_test();
            break;
        case 5:
            multimap_test();
            break;
        default:
            std::cout << "Wrong!" << std::endl;
            break;
    }
    return 0;
}
