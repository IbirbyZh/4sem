//
//  main.cpp
//  SynQ
//
//  Created by Шишкин Евгений on 13.03.16.
//  Copyright © 2016 Шишкин Евгений. All rights reserved.
//

#include <iostream>
#include "SynQ.hpp"
#include <vector>
#include <thread>
#include <list>
#include <numeric>

const int MAX_PAGE = 100;
const int TOTAL_PAGES = 1000000;

void readOrWait(SynQ<std::list<int>>& input,
                const int totalPages, int& total)
{
    total = 0;
    for(int i = 0; i < totalPages; ++i){
        int e;
        if (!input.popOrWait(e)){
            return;
        }
        total += e;
    }
}

void readNoWait(SynQ<std::list<int>>& input,
                const int totalPages, int& total)
{
    total = 0;
    for(int i = 0; i < totalPages;){
        int e;
        if (!input.popNoWait(e)){
            continue;
        }
        total += e;
        ++i;
    }
}

void writem(SynQ<std::list<int>>& input,
           const int totalPages, int& total)
{
    total = 0;
    for(int i = 0; i < totalPages; ++i){
        int e = rand() % MAX_PAGE;
        total += e;
        input.push(std::move(e));
    }
}

void test(int readersCount, int writersCount){
    SynQ<std::list<int>> input;
    std::vector<std::thread*> myWriters(writersCount);
    std::vector<std::thread*> myWait(readersCount);
    std::vector<std::thread*> myNoWait(readersCount);
    int ukW, ukWait, ukNoWait, i;
    std::vector<int> totalW(writersCount), totalWait(readersCount), totalNoWait(readersCount);
    ukW = ukWait = ukNoWait = 0;
    for(i = 0; i < std::min(readersCount, writersCount); ++i){
        myWriters[ukW] = new std::thread(writem, std::ref(input),
                                         TOTAL_PAGES / writersCount, std::ref(totalW[ukW]));
        ++ukW;
        myWait[ukWait] = new std::thread(readOrWait, std::ref(input),
                                         (TOTAL_PAGES / 2) / readersCount, std::ref(totalWait[ukWait]));
        ++ukWait;
        myNoWait[ukNoWait] = new std::thread(readNoWait, std::ref(input),
                                         (TOTAL_PAGES / 2) / readersCount, std::ref(totalNoWait[ukNoWait]));
        ++ukNoWait;
    }
    for(; i < writersCount; ++i){
        myWriters[ukW] = new std::thread(writem, std::ref(input),
                                         TOTAL_PAGES / writersCount, std::ref(totalW[ukW]));
        ++ukW;
    }
    for(; i < readersCount; ++i){
        myWait[ukWait] = new std::thread(readOrWait, std::ref(input),
                                         (TOTAL_PAGES / 2) / readersCount, std::ref(totalWait[ukWait]));
        ++ukWait;
        myNoWait[ukNoWait] = new std::thread(readNoWait, std::ref(input),
                                         (TOTAL_PAGES / 2) / readersCount, std::ref(totalNoWait[ukNoWait]));
        ++ukNoWait;
    }
    for(i = 0; i < writersCount; ++i){
        myWriters[i]->join();
    }
    input.finalize();
    for(i = 0; i < readersCount; ++i){
        myWait[i]->join();
        myNoWait[i]->join();
    }
    int a, b;
    a = b = 0;
    std::accumulate(totalW.begin(), totalW.end(), a);
    std::accumulate(totalWait.begin(), totalWait.end(), b);
    std::accumulate(totalNoWait.begin(), totalNoWait.end(), b);
    std::printf("writers: %2d, waitReaders: %2d, noWaitReaders: %2d ", readersCount, writersCount, writersCount);
    if (a == b){
        printf("OK\n");
    }else{
        printf("BAD\n");
    }
}

int main(int argc, const char * argv[]) {
    freopen("output.txt", "w", stdout);
    test(4, 2);
    test(1, 2);
    test(8, 1);
    test(16, 2);
    test(1, 4);
    return 0;
}
