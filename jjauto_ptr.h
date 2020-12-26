//
// Created by Carol on 2020/12/26.
//

#ifndef MYSTL_JJAUTO_PTR_H
#define MYSTL_JJAUTO_PTR_H
namespace jj{

    template<typename T>
    class auto_ptr{
    public:
        explicit auto_ptr(T* p = nullptr) : pointee(p) {}
        template<typename U>
        auto_ptr(auto_ptr<U>& rhs) : pointee(rhs.release()) {}
        ~auto_ptr() {delete pointee;}

        template<typename U>
        auto_ptr<T>& operator=(auto_ptr<U>& rhs){
            if(this != &rhs) reset(rhs.release());
            return *this;
        }
        T& operator*() const { return *pointee; }
        T* operator->() const { return pointee; }
        T* get() const { return pointee; }
        T* release() {
            T* tmp = pointee;
            pointee = nullptr;
            return tmp;
        }

    private:
        T* pointee;
    };
}
#endif //MYSTL_JJAUTO_PTR_H
