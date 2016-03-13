//
//  main.cpp
//  Potoki
//
//  Created by Шишкин Евгений on 19.02.16.
//  Copyright © 2016 Шишкин Евгений. All rights reserved.
//

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <numeric>
#include <chrono>
#include "Futex.hpp"

const int MAX_ANS = 3E7;

int ans;

int name(int &mySum, Futex& f)
{
    while (true) {
        f.lock();
        if (ans >= MAX_ANS){
            f.unlock();
            break;
        }
        ++ans;
        ++mySum;
        f.unlock();
    }
    return 0;
}

int nameM(int &mySum, std::mutex& f)
{
    while (true) {
        f.lock();
        if (ans >= MAX_ANS){
            f.unlock();
            break;
        }
        ++ans;
        ++mySum;
        f.unlock();
    }
    return 0;
}

void test(const char c, int threadsCount)
{
    ans = 0;
    std::vector<std::thread*> threads;
    std::vector<int> localSum(threadsCount, 0);
    std::chrono::time_point<std::chrono::system_clock> start, end;
    if (c == 'f'){
        Futex f;
        start = std::chrono::system_clock::now();
        for(int i = 0; i < threadsCount; ++i){
            threads.push_back(new std::thread
                              (name, std::ref(localSum[i]), std::ref(f)));
        }
        for(int i = 0; i < threadsCount; ++i){
            threads[i] -> join();
        }
        end = std::chrono::system_clock::now();
        std::cout << c << ' ' << threadsCount << '\n';
        for(int i = 0; i < threadsCount; ++i){
            delete threads[i];
            std::cout << localSum[i] << ' ';
        }
        double l = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()
        / static_cast<double>(1000);
        std::cout << '\n' << ans <<'\n' << std::accumulate(localSum.begin(), localSum.end(), 0) <<
        "\nTime: " << l << "\n\n";
    }else{
        std::mutex m;
        start = std::chrono::system_clock::now();
        for(int i = 0; i < threadsCount; ++i){
            threads.push_back(new std::thread
                              (nameM, std::ref(localSum[i]), std::ref(m)));
        }
        for(int i = 0; i < threadsCount; ++i){
            threads[i] -> join();
        }
        end = std::chrono::system_clock::now();
        std::cout << c << ' ' << threadsCount << '\n';
        for(int i = 0; i < threadsCount; ++i){
            delete threads[i];
            std::cout << localSum[i] << ' ';
        }
        double l = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()
                                                                / static_cast<double>(1000);
        std::cout << '\n' << ans <<'\n' << std::accumulate(localSum.begin(), localSum.end(), 0) <<
        "\nTime: " << l << "\n\n";
    }
}

int main(int argc, const char * argv[]) {
    freopen("output.txt", "w", stdout);
    std::vector<int> num;
    num.push_back(std::thread::hardware_concurrency() / 2);
    num.push_back(std::thread::hardware_concurrency());
    num.push_back(std::thread::hardware_concurrency() * 2);
    std::vector<char> c;
    c.push_back('f');
    c.push_back('m');
    for(int i = 0; i < c.size(); ++i){
        for(int j = 0; j < num.size(); ++j){
            test(c[i], num[j]);
        }
    }
    return 0;
}
