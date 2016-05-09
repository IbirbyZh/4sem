//
//  main.cpp
//  Pin-pong
//
//  Created by Шишкин Евгений on 09.05.16.
//  Copyright © 2016 Шишкин Евгений. All rights reserved.
//

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <numeric>
#include <chrono>

struct Data{
    char m[65];
};

Data data;

static const int N = 1E9;

template<int pos>
void test(){
    for(int i = 0; i < N; ++i){
        ++data.m[pos];
    }
}

int main(int argc, const char * argv[]) {
    freopen("output.txt", "w", stdout);
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    std::thread a(test<0>), b(test<1>);
    a.join();
    b.join();
    end = std::chrono::system_clock::now();
    double l = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()
    / static_cast<double>(1000);
    std::cout << "without Pin-pong " << l << '\n';
    start = std::chrono::system_clock::now();
    std::thread c(test<0>), d(test<64>);
    c.join();
    d.join();
    end = std::chrono::system_clock::now();
    l = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()
    / static_cast<double>(1000);
    std::cout << "with    Pin-pong " << l << '\n';
    std::cerr <<data.m[0] <<data.m[1] <<data.m[64];
    return 0;
}
