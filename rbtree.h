//
// Created by Carol on 2020/12/30.
//

#ifndef MYSTL_RBTREE_H
#define MYSTL_RBTREE_H
#include "alloctor.h"
#include "construct.h"
#include "queue.h"
#include <utility>			//pair
#include <string.h>

namespace mystl{
    namespace detail{
        typedef bool __rb_tree_color_type;//颜色
        const __rb_tree_color_type __rb_tree_red = 1;//红色为1
        const __rb_tree_color_type __rb_tree_black = 0;//黑色为0

        struct __rb_tree_node_base{
            typedef __rb_tree_color_type color_type;
            typedef __rb_tree_node_base* base_ptr;

            //成员 data
            color_type color;//节点颜色，非黑即红
            base_ptr parent;//父节点
            base_ptr left;//指向左节点
            base_ptr right;//指向右节点

            static base_ptr minimum(base_ptr x){
                while (x->left != 0) x = x->left;//一直向左走，找到最小值
                return x;
            }

            static base_ptr maximum(base_ptr x){
                while (x->right != 0) x = x->right;//一直向右走，找到最大值
                return x;
            }
        };

        template<class Value>
        struct __rb_tree_node : public __rb_tree_node_base{
            typedef __rb_tree_node<Value>* link_type;
            Value value_field;//节点值
        };

        //基层迭代器
        struct __rb_tree_base_iterator{
            typedef __rb_tree_node_base::base_ptr       base_ptr;
            typedef bidirectional_iterator_tag          iterator_category;
            typedef ptrdiff_t                           difference_type;

            base_ptr node;//它用来与容器产生一个连结关系

            void increament(){
                if(node->right != 0){//拥有右子节点,状况1
                    node = node->right;//向右走
                    while(node->left != 0)//然后一直往左子树走到底
                        node = node->left;////既是解答
                } else{
                    //没有右子节点，状况2
                    base_ptr y = node->parent;//找出父节点
                    while(node == y->right)//如果现行节点本身是个右子节点
                    {
                        node = y;//一直向上回溯,直到不为右子节点为止
                        y = y->parent;
                    }
                    if(node->right != y){//若此时的右子节点不等于父节点
                        node = y;        //状况3，此时的父节点为解答
                    }
                    //否则此时的node为解答，状况4
                }
            }

            void decreament(){
                if(node->color == __rb_tree_red && //节点的颜色为红色
                node->parent->parent == node)//节点的祖父节点是他自己
                    node = node->right; //状况1，右子节点即为解答
                    // 以上情况发生于node为header时
                    //注意，header的右子节点即为mostright，指向整棵树的max节点
                else if(node->left != 0){//有左子节点,状况2
                        base_ptr y = node->left;    //令y指向左子节点
                        while(y->right != 0)    //当y有右子节点时
                            y = y->right;       //一直向右走到底
                        node = y;
                } else{//既非根节点，亦非左子节点
                    base_ptr y = node->parent;//状况3，找出父节点
                    while(node == y->left)//当现行节点身为左子节点
                    {
                        node = y;//一直向上走,直到现行节点不为左子节点
                        y = y->parent;
                    }
                    node = y;   //此时之父节点为答案
                }
            }
        };

        template<class Value, class Ref, class Ptr>
        struct __rb_tree_iterator : public __rb_tree_base_iterator{
            typedef Value                                                       value_type;
            typedef Ref                                                         reference;
            typedef Ptr                                                         pointer;
            typedef __rb_tree_iterator<Value, Value&, Value*>                   iterator;
            typedef __rb_tree_iterator<Value, const Value&, const Value*>       const_iterator;
            typedef __rb_tree_iterator<Value, Ref, Ptr>                         self;
            typedef __rb_tree_node<Value>*                                      link_type;

            __rb_tree_iterator() {}
            __rb_tree_iterator(link_type x) { node = x; }
            __rb_tree_iterator(const iterator& it) { node = it.node; }

            reference operator*() const { return link_type(node)->value_field; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
            pointer operator->() const { return &(operator*()); }
#endif

            self& operator++(){
                increament();
                return *this;
            }

            self operator++(int){
                self tmp = *this;
                increament();
                return tmp;
            }

            self& operator--(){
                decreament();
                return *this;
            }

            self operator--(int){
                self tmp = *this;
                decreament();
                return tmp;
            }

            bool operator==(const iterator& x) const { return (node == x.node); }
            bool operator!=(const iterator& x) const { return (node != x.node); }
            bool operator==(const const_iterator& x) const { return (node == x.node); }
            bool operator!=(const const_iterator& x) const { return (node != x.node); }
        };

        void __rb_tree_rotate_left(__rb_tree_node_base*, __rb_tree_node_base*&);
        void __rb_tree_rotate_right(__rb_tree_node_base*, __rb_tree_node_base*&);
        void __rb_tree_rebalance(__rb_tree_node_base*, __rb_tree_node_base*&);

        template<class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
        class rb_tree{
        protected:
            typedef void*                                                   void_pointer;
            typedef __rb_tree_node_base*                                    base_ptr;
            typedef __rb_tree_node<Value>                                   rb_tree_node;
            typedef simple_alloc<rb_tree_node, Alloc>                       rb_tree_node_allocator;
            typedef __rb_tree_color_type                                    color_type;
            typedef rb_tree<Key, Value, KeyOfValue, Compare, Alloc>         self;
        public:
            typedef Key					                                                                        key_type;
            typedef Value				                                                                        value_type;
            typedef value_type*			                                                                        pointer;
            typedef const value_type*	                                                                        const_pointer;
            typedef value_type&			                                                                        reference;
            typedef const value_type&	                                                                        const_reference;
            typedef rb_tree_node*		                                                                        link_type;
            typedef size_t				                                                                        size_type;
            typedef ptrdiff_t			                                                                        difference_type;
            typedef typename __rb_tree_iterator<value_type, reference, pointer>::iterator		                iterator;
            typedef typename __rb_tree_iterator<value_type, const reference, const pointer>::const_iterator	    const_iterator;


        protected:
            link_type get_node() { return rb_tree_node_allocator::allocate(); }
            void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

            //生成一个节点
            link_type create_node(const value_type& x){
                link_type node = get_node();
                try {
                    construct(&(node->value_field), x);	//构造对象
                }
                catch (...) {
                    put_node(node);
                }
                return node;
            }

            /* 复制一个节点（颜色和值） */
            link_type clone_node(const link_type x) {
                link_type node = create_node(x->value_field);
                node->color = x->color;
                node->left = nullptr;
                node->right = nullptr;
                return node;
            }

            //销毁一个节点
            void destory_node(link_type p){
                destroy(&p->value_field);   //析构内容
                put_node(p);                //释放节点
            }

        protected:
            ///* RB_tree 只用以下三笔数据表现 */
            size_type node_count;//节点数量
            link_type header;//实现技巧
            Compare key_compare;//节点间的键值比较规则

            //三个函数用来取得header的成员
            link_type& root() const { return (link_type&)header->parent; }
            link_type& leftmost() const { return (link_type&)header->left; }
            link_type& rightmost() const { return (link_type&)header->right; }

            //6个函数用来取得节点x的成员
            static link_type& left(link_type x){ return (link_type&)x->left; }
            static link_type& right(link_type x){ return (link_type&)x->right; }
            static link_type& parent(link_type x) { return (link_type&)x->parent; }
            static reference value(link_type x) { return x->value_field; }
            static const Key& key(link_type x) { return KeyOfValue()(x->value_field); }
            static color_type& color(link_type x) { return (color_type&)x->color; }

            //取得极大值和极小值
            static link_type minimum(link_type x){
                return (link_type) __rb_tree_node_base::minimum(x);
            }
            static link_type maximum(link_type x){
                return (link_type) __rb_tree_node_base::maximum(x);
            }

        private:
            iterator __insert(base_ptr x, base_ptr y, const value_type& v);
            link_type __copy(link_type x, link_type p);
            void init(){
                header = get_node();//产生一个节点空间，令node指向它
                color(header) = __rb_tree_red;//令他的颜色为红
                root() = 0;
                leftmost() = header;    //令header的左右节点都为他自己
                rightmost() = header;
            }

        public:
            //构造/析构
            rb_tree(const Compare& comp = Compare()) : node_count(0), key_compare(comp)
            { init(); }
            rb_tree(const self& x) {		//拷贝构造函数
                init();
                *this = x;					//调用 operator=
            }
            ~rb_tree(){
                clear();
                put_node(header);
            }

            self& operator=(const self& x);	//拷贝赋值运算符


        public:
            Compare key_comp() const { return key_compare; }
            iterator begin() { return leftmost(); }
            iterator end() { return rightmost(); }
            bool empty() const { return node_count == 0; }
            size_type size() const { return node_count; }
            size_type max_size() const { return size_type (-1); }

        public:
            /* 清空 rb_tree(除header) */
            void clear();
            /* 将树*this和x交换，只需要交换header指针即可 */
            void swap(self& x) {
                link_type tmp = header;
                *(&header) = x.header;
                *(&(x.header)) = tmp;
            }

            /* 寻找RB_tree中是否存在键值为 k 的节点 */
            const_iterator find(const Key& k) const;
            /* 注意以下代码复用的技巧 */
            iterator find(const Key& k) {
                const_iterator it = static_cast<const self&>(*this).find(k);
                link_type p = static_cast<link_type>(it.node);
                return iterator(p);
            }

            /* 统计元素x的个数 */
            size_type count(const key_type& x) const {
                size_type x_count = 0;
                const_iterator it = find(x);		//当有多个相同元素时，find 返回第一个元素的迭代器

                if (it != end()) {
                    ++x_count;

                    //向后遍历查看是否还存在键值为key的元素
                    while (++it != end()) {
                        if (!key_compare(x, key(it.node)) && !key_compare(key(it.node), x))	 // !(a<b) && !(b<a) 即 a==b
                            ++x_count;
                        else
                            break;
                    }
                }
                return x_count;
            }

            /* 插入新值：键值不允许重复，重复则插入失败 */
            std::pair<iterator, bool> insert_unique(const value_type& v);

            template<class InputIterator>
            void insert_unique(InputIterator first, InputIterator last) {
                while (first != last) {
                    insert_unique(*first);
                    ++first;
                }
            }

            /* 插入新值：键值允许重复 */
            iterator insert_equal(const value_type& v);

            template<class InputIterator>
            void insert_equal(InputIterator first, InputIterator last) {
                while (first != last) {
                    insert_equal(*first);
                    ++first;
                }
            }

            iterator lower_bound(const key_type& x) { return lower_bound(begin(), end(), x); }
            const_iterator lower_bound(const key_type& x) const { return lower_bound(begin(), end(), x); }
            iterator upper_bound(const key_type& x) { return upper_bound(begin(), end(), x); }
            const_iterator upper_bound(const key_type& x) const { return upper_bound(begin(), end(), x); }

            /* 返回元素 x 的区间 */
            std::pair<iterator, iterator> equal_range(const key_type& x) {
                iterator first = find(x);
                iterator last = find(x);
                while (last != end()) {
                    if (!key_compare(x, key(last.node)) && !key_compare(key(last.node), x))
                        ++last;
                    else
                        break;
                }

                return std::pair<iterator, iterator>(first, last);
            }
        };


        //函数定义
        /* 拷贝赋值运算符 */
        template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
        rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
        rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::operator=(const self& x) {
            clear();		//先将原始的树清空
            key_compare = x.key_compare;
            header->parent = __copy(x.root());
            if (header->parent != nullptr)
            {
                root()->parent = header;
                node_count = x.node_count;

                leftmost() = get_min_node(root());
                rightmost() = get_max_node(root());
            }

            return *this;
        }

        /* 清空rb_tree(除header) */
        template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
        void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::clear()
        {
            if (header->parent != nullptr) {
                queue<base_ptr> node_queue;

                node_queue.push(header->parent);
                while (!node_queue.empty()) {
                    base_ptr cur = node_queue.front();
                    node_queue.pop();

                    if (cur->left != nullptr)
                        node_queue.push(cur->left);
                    if (cur->right != nullptr)
                        node_queue.push(cur->right);
                    destroy_node(link_type (cur));
                }

                /* 将 rb_tree 恢复到空状态 */
                node_count = 0;
                root() = nullptr;		//root 为空，即 header->parent
                leftmost() = header;	//令 header 的左右子节点为自己
                rightmost() = header;
            }
        }

        /* 寻找RB_tree中是否存在键值为 k 的节点 */
        template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
        typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator
        rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) const {
            link_type y = header;           //Last node which is not less than k
            link_type x = root();           //current node

            //注意：当存在相同元素时，find 返回第一个元素的迭代器（递增迭代器可访问其他相同元素）

            while (x != nullptr) {
                if (key_compare(key(x), k))
                    x = right(x);			//运行到这里表示 x 键值小于 k，x 向右走
                else if ( key_compare(k, key(x)) )
                    x = left(x);			//运行到这里表示 x 键值大于 k，x 向左走
                else
                    return iterator(x);		//返回第一个目标元素
            }
            return end();
        }

        /* 插入新值：节点值允许重复 */
        /* 返回指向新插入节点的迭代器 */
        template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
        typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
        rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
                insert_equal(const value_type& v) {
            /* 注意：当插入相同元素时，新元素成为已有元素的后继（即已有元素右孩子的最左节点） */
            link_type y = header;
            link_type x = root();		//从根节点开始往下寻找适当的插入点
            while (x != nullptr) {
                y = x;
                x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);		//less
                //遇大则往左，否则往右
            }
            return __insert(x, y, v);
            // x为新值插入点，y为插入点的父节点，v为待插入值
        }

        /* 插入新值：节点值不允许重复，重复则插入无效 */
        /* 返回一个pair，第一个元素指向RB_tree中新增节点，第二个元素表示是否插入成功 */
        template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
        std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
        rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
                insert_unique(const value_type& v) {
            link_type y = header;
            link_type x = root();
            bool comp = true;

            while (x != nullptr) {		//从根节点开始，向下寻找适当的插入点
                y = x;

                comp = key_compare(KeyOfValue()(v), key(x));		//less
                x = comp ? left(x) : right(x);
            } //结束后，x指向插入点，y指向x的父节点

            iterator j = iterator(y);       //令迭代器j指向插入点父节点y

            /* 如果comp为真，表示 v < x */
            if (comp) {
                if (j == begin())	//如果插入节点的父节点为 left_most，则直接插入
                    return std::pair<iterator, bool>(__insert(x, y, v), true);
                else		//否则将调整 j 用于后面测试
                    --j;
            }
            /* 如果comp为假，表示 v >= x，如果是 v > x 则进行插入 */
            if (key_compare(key(j.node), KeyOfValue()(v))) {
                return std::pair<iterator, bool>(__insert(x, y, v), true);
            }

            /* 进行到这里，键值一定重复，插入失败 */
            /* 注意：这里有个技巧是返回一个第一元素指向发生冲突 */
            /* 的节点的迭代器，在map的下标操作中起了重要的作用 */
            return std::pair<iterator, bool>(j, false);
        }

        /* 真正的插入程序: x为新值插入点，y为插入点的父节点，v为待插入值。返回指向新插入节点的迭代器 */
        template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
        typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
        rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
                __insert(base_ptr _x, base_ptr _y, const value_type& v) {
            link_type x = (link_type)_x;
            link_type y = (link_type)_y;

            link_type new_node = create_node(v);			//产生一个新节点
            if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y)))
            {
                left(y) = new_node;		//这使得当y为header时，leftmost() = new_node
                if (y == header) {
                    root() = new_node;
                    rightmost() = new_node;
                }
                else if (y == leftmost()) {
                    leftmost() = new_node;
                }
            }
            else {
                right(y) = new_node;		//令新节点为插入点父节点的右子节点
                if (y == rightmost()) {
                    rightmost() = new_node;		//维护 rightmost 永远指向最右节点
                }
            }
            parent(new_node) = y;		//设定新节点的父节点
            left(new_node) = 0;
            right(new_node) = 0;		//设定新节点的左右子节点

            //节点颜色一定为红，将在全局函数__rb_tree_rebalance() 中设定并调整
            __rb_tree_rebalance(new_node, header->parent);		//参数一为新增节点，参数二为 root
            ++node_count;
            return iterator(new_node);		//返回指向新增节点的迭代器
        }


        /* 全局函数：调整树形使其平衡（改变颜色及旋转树形） */
        /* 参数一为新增节点，参数二为 root */
        inline void
        __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
            x->color = __rb_tree_red;		//新节点必为红（根据规则4）

            while (x != root && x->parent->color == __rb_tree_red)	//父节点也为红色
            {
                if (x->parent == x->parent->parent->left)		//父节点为祖父节点的左子节点
                {
                    __rb_tree_node_base* y = x->parent->parent->right;	//令 y 为伯父节点

                    if (y && y->color == __rb_tree_red)			//伯父节点存在且为红色（对应P211状况4）
                    {
                        y->color = __rb_tree_black;				//更改伯父节点为黑
                        x->parent->color = __rb_tree_black;		//更改父节点为黑
                        x->parent->parent->color = __rb_tree_red;	//更改祖父节点为红

                        x = x->parent->parent;		/* x 上升到祖父节点，下次继续判断祖父节点和太祖父节点是否同时为红 */
                    }
                    else	//无伯父节点，或伯父节点为黑
                    {
                        if (x == x->parent->right)			//如果新节点为父节点的右子节点（对应P210状况2）
                        {
                            x = x->parent;					//修改 x 为其父节点，先对其父节点进行一次左旋操作
                            __rb_tree_rotate_left(x, root);
                        }
                        x->parent->color = __rb_tree_black;			//将父节点改为黑
                        x->parent->parent->color = __rb_tree_red;	//将祖父节点改为红
                        __rb_tree_rotate_right(x->parent->parent, root);	//对祖父节点执行右旋

                        /* 此时x和兄弟节点(原祖父节点)为红，x父节点为黑，已满足rb_tree要求，下次循环将退出while */
                    }
                }
                else		//父节点为祖父节点的右子节点
                {
                    __rb_tree_node_base* y = x->parent->parent->left;	//令 y 为伯父节点

                    if (y && y->color == __rb_tree_red)		//伯父节点存在且为红色（对应上述状况4的镜像）
                    {
                        y->color = __rb_tree_black;				//更改伯父节点为黑
                        x->parent->color = __rb_tree_black;		//更改父节点为黑
                        x->parent->parent->color = __rb_tree_red;	//更改祖父节点为红

                        x = x->parent->parent;		/* x 上升到祖父节点，下次继续判断祖父节点和太祖父节点是否同时为红 */
                    }
                    else	///无伯父节点，或伯父节点为黑
                    {
                        if (x == x->parent->left)		//如果新节点为父节点的左子节点（对应上述状况2的镜像）
                        {
                            x = x->parent;				//修改 x 为其父节点，先对其父节点进行一次右旋操作
                            __rb_tree_rotate_right(x, root);
                        }
                        x->parent->color = __rb_tree_black;			//将父节点改为黑
                        x->parent->parent->color = __rb_tree_red;	//将祖父节点改为红
                        __rb_tree_rotate_left(x->parent->parent, root);		//对祖父节点执行左旋

                        /* 此时x和兄弟节点(原祖父节点)为红，x父节点为黑，已满足rb_tree要求，下次循环将退出while */
                    }
                }
            }		//while 结束
            root->color = __rb_tree_black;		//根节点永远为黑
        }

/* 全局函数：x为左旋点，root为根节点 */
/* 新节点必为红节点，如果插入点的父节点也为红节点，则可能需要进行树形旋转及颜色改变 */
        inline void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
            __rb_tree_node_base* y = x->right;		//令 y 为左旋点的右子节点
            x->right = y->left;
            if (y->left != nullptr) {
                y->left->parent = x;		//别忘了设定父节点
            }
            y->parent = x->parent;

            //令 y 完全顶替 x 的地位（必须将 x 对其父节点的关系完全接收过来）
            if (x == root) {					//如果 x 为根节点，将 y 设为新的根节点
                root = y;
                x->parent->parent = y;			//令header的parent指向新的根节点
            }
            else if (x == x->parent->left)		//x 为其父节点的左子节点
                x->parent->left = y;
            else								//x 为其父节点的右子节点
                x->parent->right = y;
            y->left = x;
            x->parent = y;
        }

/* 全局函数：x为右旋点，root为根节点 */
/* 新节点必为红节点，如果插入点的父节点也为红节点，则可能需要进行树形旋转及颜色改变 */
        inline void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
            __rb_tree_node_base* y = x->left;		//令 y 为右旋点的左子节点
            x->left = y->right;
            if (y->right != nullptr) {
                y->right->parent = x;		//别忘了设定父节点
            }
            y->parent = x->parent;

            //令 y 完全顶替 x 的地位（必须将 x 对其父节点的关系完全接收过来）
            if (x == root) {					//如果 x 为根节点，将 y 设为新的根节点
                root = y;
                x->parent->parent = y;			//令header的parent指向新的根节点
            }
            else if (x == x->parent->right)		//x 为其父节点的右子节点
                x->parent->right = y;
            else								//x 为其父节点的左子节点
                x->parent->left = y;
            y->right = x;
            x->parent = y;
        }
    }// end of namespace detail
}
#endif //MYSTL_RBTREE_H
