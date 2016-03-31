//
//  StackLockFree.hpp
//  LockFree
//
//  Created by Шишкин Евгений on 25.03.16.
//  Copyright © 2016 Шишкин Евгений. All rights reserved.
//

#ifndef StackLockFree_hpp
#define StackLockFree_hpp

#include <stdio.h>
#include <atomic>

template <class T>
class StackLockFree {
public:
    StackLockFree();
    void push(const T& data);
    bool pop(T& data);
private:
    struct node{
        node();
        node(const T& _data);
        node(T&& _data);
        T data;
        node* next;
    };
    std::atomic<node*> head;
};

template<class T>
StackLockFree<T>::node::node():T(), next(NULL){}
template<class T>
StackLockFree<T>::node::node(const T& _data):data(_data), next(NULL){}
template<class T>
StackLockFree<T>::node::node(T&& _data):data(std::move(_data)), next(NULL){}

template<class T>
StackLockFree<T>::StackLockFree():head(NULL){}

template<class T>
void StackLockFree<T>::push(const T& data){
    node* newNode = new node(data);
    newNode->next = head.load();
    while(!head.compare_exchange_strong(newNode->next, newNode));
}

template<class T>
bool StackLockFree<T>::pop(T& data){
    node* a = head.load();
    do{
        if(a == NULL){
            return false;
        }
    }while(!head.compare_exchange_strong(a, a->next));
     
 //   data = std::move(a->data);
    data = a->data;
    delete a;
    return true;
     
}

#endif /* StackLockFree_hpp */
