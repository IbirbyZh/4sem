//
//  Futex.cpp
//  Potoki
//
//  Created by Шишкин Евгений on 04.03.16.
//  Copyright © 2016 Шишкин Евгений. All rights reserved.
//

#include "Futex.hpp"
#include <thread>
#include <assert.h>

Futex::Futex()
{
    ownerId.operator=(-1);
}

bool Futex::lock()
{
    std::thread::id id = std::this_thread::get_id();
    int a = std::hash<std::thread::id>()(id) / 1;
    int f = -1;
    while (!ownerId.compare_exchange_weak(f, a, std::memory_order_acquire)){
        f = -1;
    }
    return true;
}

bool Futex::unlock()
{
    std::thread::id id = std::this_thread::get_id();
    int a = std::hash<std::thread::id>()(id) / 1;
    int f = a;
    if (!ownerId.compare_exchange_weak(f, -1, std::memory_order_release)){
        assert("Ошибка");
    }
    return true;
}