#pragma once
#include <iostream>
#include <stdlib.h>
#include <string>
#include <chrono>
#include <vector>

class SVTimer
{
private:
    std::chrono::_V2::system_clock::time_point now;
    std::chrono::_V2::system_clock::time_point last_time;
    std::chrono::nanoseconds dt;
    int dtMs;

    std::vector<std::string> names;
    std::vector<int> times;

public:
    SVTimer(){
        last_time = std::chrono::high_resolution_clock::now();
    }

    ~SVTimer() {}

    void reset(){
        now = std::chrono::high_resolution_clock::now();
        last_time = now;
        times.clear();
        names.clear();
    }

    void update(std::string name){
        now = std::chrono::high_resolution_clock::now();
        dt = now - last_time;
        last_time = now;
        dtMs = std::chrono::duration_cast<std::chrono::milliseconds>(dt).count();
        times.push_back(dtMs);
        names.push_back(name);
    }


    int print(std::string text){
        printf("[ %s ] ", text.c_str());
        int totalMs = 0;
        for (size_t i=0; i<times.size(); i++) {
            printf(" %s: %d ms, ", names[i].c_str(), times[i]);
            totalMs += times[i];
        }
        printf(" total = %d ms\n", totalMs);
        reset();
        return totalMs;
    }
};