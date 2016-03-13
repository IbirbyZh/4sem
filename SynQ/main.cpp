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

void sort(SynQ<std::list<std::vector<int>>>& input,
          SynQ<std::list<std::vector<int>>>& conveyor,
          SynQ<std::list<std::pair<int, int>>>& output)
{
    int id = std::hash<std::thread::id>()(std::this_thread::get_id()) / 1;
    for(int i = 0;true;++i){
        std::vector<int> array;
        if (!input.popOrWait(array)){
            return;
        }
        std::sort(array.begin(), array.end());
        output.push(std::make_pair(id, i));
        array.clear();
        conveyor.push(std::move(array));
    }
}

int main(int argc, const char * argv[]) {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    SynQ<std::list<std::vector<int>>> input, conveyor(4);
    SynQ<std::list<std::pair<int, int>>> output;
    int threadsCount = std::thread::hardware_concurrency() / 2;
    std::vector<std::thread*> myThreads(threadsCount);
    for(auto i = myThreads.begin(); i < myThreads.end(); ++i){
        *i = new std::thread(sort,
                             std::ref(input), std::ref(conveyor), std::ref(output));
    }
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i){
        int m;
        scanf("%d", &m);
        std::vector<int> array;
        conveyor.popOrWait(array);
        for(int j = 0; j < m; ++j){
            int x;
            scanf("%d", &x);
            array.push_back(std::move(x));
        }
        input.push(std::move(array));
    }
    input.finalize();
    for(int i = 0; i < n; ++i){
        std::pair<int, int> ans;
        output.popOrWait(ans);
        printf("thread:%d count:%d\n", ans.first, ans.second);
    }
    for(auto i = myThreads.begin(); i < myThreads.end(); ++i){
        (*i)->join();
    }
    return 0;
}
