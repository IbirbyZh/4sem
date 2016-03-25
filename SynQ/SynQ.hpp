//
//  SynQ.hpp
//  SynQ
//
//  Created by Шишкин Евгений on 13.03.16.
//  Copyright © 2016 Шишкин Евгений. All rights reserved.
//

#ifndef SynQ_hpp
#define SynQ_hpp

#include <stdio.h>
#include <mutex>
#include <condition_variable>

#include <queue>
#include <stack>
#include <utility>

template<typename T>
struct isConteiner{
private:
    static int detect(...);
    template<typename U> static decltype(std::declval<U>().pop()) detect(const U&);
public:
    static constexpr bool value = !(std::is_same<void, decltype(detect(std::declval<T>()))>::value);
};

template<typename T>
struct isStack{
private:
    static void detect(...);
    template<typename U> static decltype(std::declval<U>().top()) detect(const U&);
public:
    static constexpr bool value = !(std::is_same<void, decltype(detect(std::declval<T>()))>::value);
};

//можно изпользовать с std::deque, std::list, std::vector
template <class ARRAY>
class SynQ {
public:
    typedef typename ARRAY::value_type value_type;
    SynQ():end(false){}
    SynQ(size_t n):queue(n), end(false){}
    void push(value_type&& e)
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        //queue.push_back(std::move(e));
        myPush(t<isConteiner<ARRAY>::value, isStack<ARRAY>::value>(), std::move(e));
        mlock.unlock();
        _cond.notify_one();
    }
    int popOrWait(value_type& e)
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        while (queue.empty() && !end) {
            _cond.wait(mlock);
        }
        if (queue.empty()){
            return 0;
        }
//        e = std::move(queue.back());
//        queue.pop_back();
        e = std::move(myBack(t<isConteiner<ARRAY>::value, isStack<ARRAY>::value>()));
        myPop(t<isConteiner<ARRAY>::value, isStack<ARRAY>::value>());
        return 1;
    }
    int popNoWait(value_type& e)
    {
        std::lock_guard<std::mutex> mlock(_mutex);
        if (queue.empty()){
            return 0;
        }
//        e = std::move(queue.back());
//        queue.pop_back();
        e = std::move(myBack(t<isConteiner<ARRAY>::value, isStack<ARRAY>::value>()));
        myPop(t<isConteiner<ARRAY>::value, isStack<ARRAY>::value>());
        return 1;
    }
    void push(const value_type& e)
    {
        std::unique_lock<std::mutex> mlock(_mutex);
//        queue.push_back(e);
        myPush(t<isConteiner<ARRAY>::value, isStack<ARRAY>::value>(), e);
        mlock.unlock();
        _cond.notify_one();
    }
    void finalize()
    {
        end = true;
        _cond.notify_all();
    }
private:
    std::mutex _mutex;
    std::condition_variable _cond;
    ARRAY queue;
    bool end;
    
    template<bool a, bool b> struct t{};
    
    template<bool isContainer, bool isStack>
    value_type& myBack(t<isContainer, isStack>);
    value_type& myBack(t<1, 0>){
        return queue.back();
    }
    value_type& myBack(t<0, 0>){
        return queue.back();
    }
    value_type& myBack(t<0, 1>){
        return queue.top();
    }
    
    template<bool isContainer, bool isStack>
    void myPop(t<isContainer, isStack>);
    void myPop(t<1, 0>){
        queue.pop_back();
    }
    void myPop(t<0, 0>){
        queue.pop();
    }
    void myPop(t<0, 1>){
        queue.pop();
    }
    
    template<bool isContainer, bool isStack>
    void myPush(t<isContainer, isStack>, const value_type& e);
    void myPush(t<1, 0>, const value_type& e){
        queue.push_back(e);
    }
    void myPush(t<0, 0>, const value_type& e){
        queue.push(e);
    }
    void myPush(t<0, 1>, const value_type& e){
        queue.push(e);
    }
    
    template<bool isContainer, bool isStack>
    void myPush(t<isContainer, isStack>, value_type&& e){}
    void myPush(t<1, 0>, value_type&& e){
        queue.push_back(std::move(e));
    }
    void myPush(t<0, 0>, value_type&& e){
        queue.push(std::move(e));
    }
    void myPush(t<0, 1>, value_type&& e){
        queue.push(std::move(e));
    }
};


#endif /* SynQ_hpp */
