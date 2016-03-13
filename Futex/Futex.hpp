//
//  Futex.hpp
//  Potoki
//
//  Created by Шишкин Евгений on 04.03.16.
//  Copyright © 2016 Шишкин Евгений. All rights reserved.
//

#ifndef Futex_hpp
#define Futex_hpp

#include <stdio.h>
#include <atomic>

class Futex{
public:
    Futex();
    Futex(const Futex&) = delete;
    bool lock();
    bool unlock();
private:
    std::atomic<int> ownerId;
};

#endif /* Futex_hpp */
