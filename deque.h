//
// Created by Carol on 2020/12/29.
//

#ifndef MYSTL_DEQUE_H
#define MYSTL_DEQUE_H
#include "alloctor.h"
#include "construct.h"
#include "iterator.h"
#include "__type_traits.h"
#include "uninitialized.h"

namespace mystl{
    namespace detail{
        template<class T, class Ref, class Ptr, size_t BufSiz = 0>
        struct __deque_iterator{//deque迭代器
            typedef __deque_iterator<T, T&, T*, BufSiz>             iterator;
            typedef __deque_iterator<T, T&, T*, BufSiz>             const_iterator;
            static size_t buffer_size() { return __deque_buf_size(BufSiz, sizeof(T)); }

            typedef random_access_iterator_tag                      iterator_category;
            typedef T                                               value_type;
            typedef Ptr                                             pointer;
            typedef Ref                                             reference;
            typedef size_t                                          size_type;
            typedef ptrdiff_t                                       difference_type;
            typedef T**                                             map_pointer;

            typedef __deque_iterator                                self;

            //保持与容器的连接
            T* cur;             //指向此迭代器指向的缓冲区当前元素
            T* first;           //指向此迭代器指向的缓冲区头部
            T* last;            //指向此迭代器指向的缓冲区尾部
            map_pointer node;   //指向中控中心

            //deque迭代器关键行为
            //遇到边缘时跳到下一个buf
            void set_node(map_pointer new_node){
                node = new_node;
                first = *new_node;
                last = first + difference_type(buffer_size());
            }

            //重载
            reference operator*(){ return *cur; }
            Ptr operator->(){ return &(operator*() ); }     //这么写，大概是为了整齐统一

            difference_type operator-(const self& x){
                return difference_type (buffer_size()) * (node - x.node -1) + (cur - first) + (x.last - x.cur);
                //node - x.node -1像植树问题（B-A），表示有中间有几个缓冲区 比如第5个缓冲区-第1个缓冲区-1 = 3，中间有3个缓冲区
                //再加上cur-first(当前buf（B）的剩余元素)，x.last-x.cur(开头buf（A）的剩余元素)
            }

            self& operator++(){//前++
                ++cur;
                if(cur == last){//如果到达末尾
                    set_node(node+1);//去下一个buf
                    cur = first;
                }
                return *this;
            }
            //++的是迭代器的位置
            self operator++(int){//后++
                self tmp = *this;
                ++(*this);
                return tmp;
            }

            self& operator--(){//前--
                //为什么++不用先判断，--要先判断呢？因为cur指向的位置，cur是指向当前元素的（有点像当前元素的前面），所以先++不会越界，--会
                if(cur == first){//如果到达末尾
                    set_node(node-1);//去上一个buf
                    cur = last;
                }
                --cur;
                return *this;
            }
            //--的是迭代器的位置
            self operator--(int){//后--
                self tmp = *this;
                --(*this);
                return tmp;
            }

            self& operator+=(difference_type n){
                difference_type offset = n + (cur - first);
                if(offset >= 0 && offset < difference_type(buffer_size()))
                    //目标位置在同一缓冲区内
                    cur += n;
                else{
                    //不在同一缓冲区内
                    difference_type node_offset =
                            offset > 0 ? offset / difference_type (buffer_size())
                            : -difference_type((-offset-1) / buffer_size()) - 1;
                    //切换至正确节点
                    set_node(node_offset);
                    //切换至正确元素
                    cur = first + (offset - node_offset * difference_type(buffer_size()));
                }
                return *this;
            }

            self operator+(difference_type n){
                self tmp = *this;
                return tmp += n;
            }

            self& operator-=(difference_type n){
                return *this += -n;
            }

            self& operator-(difference_type n){
                self tmp = *this;
                return tmp -= n;
            }
            //实现随机存取
            reference operator[](difference_type n) const { return *(*this+n); } //*this是cur，*(cur+n)是里面存的元素

            bool operator==(const self& x) const {  return cur = x.cur; }
            bool operator!=(const self& x) const { return cur != x.cur; }
            bool operator<(const self& x) const{
                return (node == x.node) ? (cur < x.cur) : (node < x.node);
            }
        };//end of iterator

        //如果n不等于0，传回n，表示BufSiz由用户自定义，就表示一个buf放n个
        //如果n等于0，就使用系统默认值
        //sz（元素大小） < 512,传回512/sz   比如int，4个字节，一个buf就可以放128个int型的数据
        //>=, 传回1，说明这个数据结构太大了，一个buf就都用来放他
        inline size_t __deque_buf_size(size_t n, size_t sz){
            return n != 0 ? n : (sz < 512 ?  size_t(512 / sz) : size_t(1));
        }

    } // end of namespace detail

    //deque的结构
    template<class T, class Alloc = alloc, size_t BufSiz = 0>
    class deque{
    public:

        //维护一个指向map的指针，start和finish两个迭代器，指向第一缓冲区的第一个元素和最后一个缓冲区的最后一个元素的下一个位置,
        // 同时也必须记住mapsize,一旦map大小不够，就分配一块更大的map
    public://basic type
        typedef typename detail::__deque_iterator<T, T&, T*, BufSiz>::iterator_category         category;
        typedef typename detail::__deque_iterator<T, T&, T*, BufSize>::const_iterator		const_iterator;
        typedef typename detail::__deque_iterator<T, T&, T*, BufSize>::value_type			value_type;
        typedef typename detail::__deque_iterator<T, T&, T*, BufSize>::pointer				pointer;
        typedef typename detail::__deque_iterator<T, T&, T*, BufSize>::const_pointer		const_pointer;
        typedef typename detail::__deque_iterator<T, T&, T*, BufSize>::reference			reference;
        typedef typename detail::__deque_iterator<T, T&, T*, BufSize>::const_reference		const_reference;
        typedef typename detail::__deque_iterator<T, T&, T*, BufSize>::size_type			size_type;
        typedef typename detail::__deque_iterator<T, T&, T*, BufSize>::difference_type		difference_type;

        size_t (*buffer_size)() = detail::__deque_iterator<T, T&, T*, BufSize>::buffer_size;

    public:
        typedef typename detail::__deque_iterator<T, T&, T*, BufSiz>::iterator              iterator;//迭代器

    protected://map_poniter
        //指针的指针
        typedef pointer*              map_pointer;
    protected://Data members    数据成员
        iterator start;
        iterator finish;
        map_pointer map;
        size_type map_size;     //map内有多少指针

    public:
        iterator begin(){ return start; }
        iterator end(){ return finish; }

        reference operator[](size_type n){
            return start[difference_type(n)];
        };

        reference front(){ return *start; }
        reference back(){
            iterator tmp = finish;
            --tmp;
            return *tmp;
            //以上三行不能改为 return *(finish - 1)
            //因为 operator-(difference_type n) 的操作比 -- 复杂很多
        }

        size_type size() const { return start - finish; }
        size_type max_size() const { return size_type(-1); } //极大值，4 or 8个字节无符号数的最大值
        bool empty() const { return finish == start; }
    //内存分配与管理
    protected:
        //专属空间配置器，每次分配一个元素大小
        typedef simple_alloc<value_type, Alloc> data_allocator;     //释放缓冲区
        //专属空间配置器，每次分配一个指针大小
        typedef simple_alloc<pointer, Alloc> map_allocator;         //释放map
    public:
        deque() : start(), finish(), map(0), map_size(0){
                create_map_and_nodes(0);    //拥有0个元素、1个缓冲区、map大小为 3
        }
        deque(int n, const value_typ& value)
        :start(), finish(), map(0), map_size(0)
        {
            fill_initialize(n, value);
        }
        void create_map_and_node(size_type num_elements);
        void fill_initialize(size_type n, const value_type& value);
        void reallocate_map(size_type nodes_to_add, bool add_at_front);

    public:
        void push_back(const value_type& t);
        void push_back_aux(const value_type &t);
        void push_front(const value_type& t);
        void push_front_aux(const value_type &t);
        void reverse_map_at_back(size_type nodes_to_add = 1);
        void reverse_map_at_front(size_type nodes_to_add = 1);
        void pop_back();
        void pop_back_aux();
        void pop_front();
        void pop_front_aux();
    };

    //create_map_and_nodes()负责申请 num_elements 个元素的存储空间，并安排好deque的结构
    template<class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::create_map_and_node(size_type num_elements){
        //需要节点数 = (元素个数 / 缓冲区大小可容纳元素个数) + 1，如果刚好整除会多配一个节点设为last
        size_type num_nodes = num_elements / buffer_size() + 1;
        //一个map最少管理3个节点，最多管理所需节点数+2，前后各留一个便于扩展
        map_size = max(initial_map_size(), num_nodes+2);
        map = map_allocator::allocate(map_size);

        //以下令 nstart 和 nfinish 指向map所拥有的全部节点的最中央区段
        //保持在最中央，可使头尾两端的扩充能量一样大，每个节点可对应一个缓冲区
        map_pointer nstart = map + 1;
        map_pointer nfinish = nstart + num_nodes - 1;

        map_pointer cur;
        try{
            //为map的节点 [nstart,nfinish] 配置缓冲区，deque的可用空间为 buffer_size() * (nfinish - nstart + 1)
            for(cur = nstart; cur <= nfinish; ++cur)
                //*cur = data_allocator::allocate(buffer_size());
                *cur = data_allocator::allocate(buffer_size());
        }
        catch (...) {
            //若非全部成功，就将 [nstart, cur) 内配置完成的缓冲区全部释放
            for (map_pointer b = nstart; b < cur; ++b)
                data_allocator::deallocate(*b, buffer_size());
            //将 map 指针所指空间释放
            map_allocator::deallocate(map, map_size);
            throw;
        }
        /* 设置 start 指向第一个缓冲区、start.cur指向第一个缓冲区的第一个元素 */
        start.set_node(nstart);
        finish.set_node(nfinish);
        start.cur = start.finish;
        finish.cur = finish.first + num_elements % buffer_size();
        //前面说过，如果刚好整除，会多配一个节点
        //此时即令cur 指向这多配的一个节点（所对映之缓冲区）的起始处
    }

    //fill_initialize() 负责安排好deque的结构，并将元素设定初值
    template<class T, class Alloc, size_t BufSiz>
    void deque<T, Alloc, BufSiz>::fill_initialize(size_type n,
                                    const value_type& value){
        create_map_and_node(n);
        try{
            for(cur = start.ndoe; cur < finish,node; ++cur)
            {
                //为每个节点的缓冲区设定初值
                uninitialized_fill(*cur, *cur + buffer_size(), value);
                //最后一个节点因为尾端可能有备用空间，所以不用设初值
                uninitialized_fill(finish.start, finish.cur, value);
            }
            }
        catch(...){
            /* 先将已构造的元素销毁 */
            for (map_pointer beg = start.node; beg < cur; ++beg)
                destroy(*cur, *cur + buffer_size());
            /* 将所有缓冲区释放 */
            for (cur = start.node; cur <= finish.node; ++cur)
                data_allocator::deallocate(*cur, buffer_size());
            /* 将 map 指针所指空间释放 */
            map_allocator::deallocate(map, map_size);
            throw;
        }
    }

    /* 重新生成一个更大的 map */
    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
        size_type old_num_nodes = finish.node - start.node + 1;		//计算之前实际缓冲区个数
        size_type new_num_nodes = old_num_nodes + nodes_to_add;

        map_pointer new_nstart;
        /* 如果旧map的大于2倍实际需要的缓冲区节点数（即map有一半还没有用到） */
        if (map_size > 2 * new_num_nodes) {
            new_nstart = map + (map_size - new_num_nodes) / 2	//将缓冲区新起点移动到 map 中间位置
                         + (add_at_front ? nodes_to_add : 0);
            /* 如果新起点在原起点的前面 */
            if (new_nstart < start.node) {
                copy(start.node, finish.node + 1, new_nstart);
                /* 将原缓冲区信息向前拷贝 */
            }
            else {
                copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
                /* 将原缓冲区信息向后拷贝 */
            }
        }
        else {		//重新配置一块空间作为 map
            size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
            map_pointer new_map = map_allocator::allocate(new_map_size);
            new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                         + (add_at_front ? nodes_to_add : 0);

            //接下来把原map内容拷贝过来，释放原map，设定新map的起始地址和大小
            copy(start.node, finish.node + 1, new_nstart);
            map_allocator::deallocate(map, map_size);
            map = new_map;
            map_size = new_map_size;
        }
        //更新 start 和 finish
        start.set_node(new_nstart);
        finish.set_node(new_nstart + old_num_nodes - 1);
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_back(const value_type& t){
        if(finish.cur != finish.last-1){
            //最后缓冲区尚有一个以上的备用空间
            construct(finish.cur, t);   //直接在备用空间上构造
            +finish.cur;                //调整cur
        } else{//最后缓冲区已无（或只剩一个）备用空间
            push_back_aux(t);
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_back_aux(const value_type &t) {
        value_type t_copy = t;
        reserve_map_at_back();		//当 map 的备用空间已用完时，无法再增加缓冲区节点，必须换一个更大的 map
        *(finish.node + 1) = data_allocator::allocate(buffer_size());	   //配置一个新节点
        try{
            construct(finish.cur, t_copy);  //在原始 finish 所指缓冲区的最后一个存储位置构造元素
            finish.setnode(finish.node+1);  //更新 finish 所指节点
            finish.cur = finish.first;      //更新cur
        }
        catch (...) {//发生异常，将新配置的节点(finish所指节点的下一个节点)释放
            data_allocator::deallocate(*(finish.node + 1));
            throw;
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_front(const value_type &t) {
        if(start.cur != start.first) {   //第一缓冲区尚有备用空间
            construct(start.cur - 1, t);  //直接构造
            --start.cur;
        }else{
            push_front_aux(t);
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_front_aux(const value_type &t) {
        value_type t_copy = t;
        reserve_map_at_front();     //若符合某种条件就要换一个map
        *(start.node - 1) = data_allocator::allocate(buffer_size());	   //配置一个新节点
        try{
            start.set_node(start.node - 1);		//更新 start 指向新节点
            start.cur = start.last - 1;			//设置 start 的状态
            construct(start.cur, t_copy);		//构造元素
        }
        catch (...) {//发生异常，释放新节点，恢复到原始状态
            start.set_node(start.node + 1);
            start.cur = start.first;
            data_allocator::deallocate(*(start.node - 1));
            throw;
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::reverse_map_at_back(size_type nodes_to_add) {
        if(nodes_to_add + 1 > map_size - (finish.node - map))
        // 如果map尾端的节点备用空间不足
        //符合以上条件则必须重换一个map(配置更大的，拷贝原来的，释放原来的)
            reallocate_map(nodes_to_add, false);
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::reverse_map_at_front(size_type nodes_to_add) {
        if(nodes_to_add > start.node - map)
            //map前端备用节点不足
            reallocate_map(nodes_to_add, true);
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::pop_back() {
        if(finish.first != finish.cur) {
            //最后缓冲区有一个或多个元素
            --finish.cur;           //调整指针，指向最后一个元素
            destroy(finish.cur);    //析构最后一个元素
        } else{
            //最后缓冲区里没有任何元素
            pop_back_aux();
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::pop_back_aux() {
        data_allocator::deallocate(finish.first);   //释放最后一个缓冲区
        finish.set_node(finish.node - 1);           //调整finish的指向，指向上一个buf
        finish.cur = finish.last - 1;               //调整cur
        destroy(finish.cur);                        //析构
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::pop_front() {
        if(start.last - 1 != start.cur){
            ////第一缓冲区有至少两个元素
            destroy(start.cur);     //析构
            ++start.cur;            //调整指针
        } else{
            //第一缓冲区只有一个元素
            pop_front_aux();
        }
    }

    template<class T, class Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::pop_front_aux() {
        destroy(start.cur);     //将第一缓冲区的最后一个元素析构
        data_allocator::deallocate(start.first);    //释放第一缓冲区
        start.set_node(start.node + 1);             //调整node
        start.cur = start.first;                    //调整cur
    }
}
#endif //MYSTL_DEQUE_H
