//
//  main.cpp
//  LockFree
//
//  Created by Шишкин Евгений on 25.03.16.
//  Copyright © 2016 Шишкин Евгений. All rights reserved.
//

#include <iostream>
#include "StackLockFree.hpp"
#include <vector>
#include <thread>
#include <list>
#include <numeric>

const int ITER_NUM = 1E5;
const int MAX = 1E4;

void test(StackLockFree<int>& slt, int& pushSum, int& popSum, int& noPop){
    pushSum = popSum = 0;
    std::list<int> a, b;
    for(int i = 0; i < ITER_NUM; ++i){
        if(rand() % 2 == 0){
            int x = rand() % MAX;
            pushSum += x;
            slt.push(x);
            a.push_front(x);
        }else{
            int x;
            if(slt.pop(x)){
                popSum += x;
                b.push_front(x);
            }else{
                ++noPop;
            }
        }
    }
    int x;
    while(slt.pop(x)){
        popSum += x;
    }
}

bool mainTest(){
    StackLockFree<int> slt;
    int threadsNum = std::thread::hardware_concurrency();
    std::vector<std::thread*> threads(threadsNum);
    std::vector<int> pushSum(threadsNum), popSum(threadsNum), noPop(threadsNum);
    for(int i = 0; i < threadsNum; ++i){
        threads[i] = new std::thread(
                                     test, std::ref(slt), std::ref(pushSum[i]), std::ref(popSum[i]), std::ref(noPop[i]));
    }
    for(int i = 0; i < threadsNum; ++i){
        threads[i] ->join();
        delete threads[i];
    }
    int sumPop = std::accumulate(popSum.begin(), popSum.end(), 0);
    int sumPush = std::accumulate(pushSum.begin(), pushSum.end(), 0);
    int sumNo = std::accumulate(noPop.begin(), noPop.end(), 0);
    printf("%d = %d\nNo pop : %d\n", sumPop, sumPush, sumNo);
    if(sumPop == sumPush){
        printf("OK\n\n");
        return true;
    }else{
        printf("WRONG\n\n");
        return false;
    }
}

int main(int argc, const char * argv[]) {
    int ans = 0;
    for(int i = 0; i < 10; ++i){
        if(!mainTest()){
            ++ans;
        }
    }
    printf("%d\n", ans);
    return 0;
}
