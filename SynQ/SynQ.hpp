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
        queue.push_back(std::move(e));
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
        e = std::move(queue.back());
        queue.pop_back();
        return 1;
    }
    int popNoWait(value_type& e)
    {
        std::lock_guard<std::mutex> mlock(_mutex);
        if (queue.empty()){
            return 0;
        }
        e = std::move(queue.back());
        queue.pop_back();
        return 1;
    }
    void push(const value_type& e)
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        queue.push_back(e);
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
};

#endif /* SynQ_hpp */
