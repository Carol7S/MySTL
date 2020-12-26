//
// Created by Carol on 2020/12/26.
//
/*
 *这里定义了一些全局函数，用来填充、复制大块内存数据。
 *
 *主要定义了uninitialized_copy、uninitialized_fill和uninitialized_fill_n，
 *
 *它们都具有“commit or rollback”语意，意思是要么成功构造出所有必要元素，
 *要么不构造任何东西（当有任何一个constructor失败时，需要rollback）
 */
#ifndef MYSTL_UNINITIALIZED_H
#define MYSTL_UNINITIALIZED_H

#endif //MYSTL_UNINITIALIZED_H
