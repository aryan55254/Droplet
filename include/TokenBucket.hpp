#pragma once

#include <chrono>
#include <mutex>
#include <algorithm>

using TimePoint = std::chrono::steady_clock::time_point;

class TokenBucket
{
private:
    mutable std::mutex mux;
    double tokens;             // tokens present currently in the bucket
    TimePoint last_check_time; //  last time bucket was checked to be filled
    double capacity;           // capacity of the bucket
    double rate;               // tokens per second

    TimePoint last_accessed; // This stores when the client last tried to use the API

    void refill(); // function to refill the bucket with per second rate prieciion

public:
    TokenBucket(double c, double r);
    bool allow(int n);
    TimePoint getLastAccessedTime() const;
};