#pragma once

#include <mutex>
#include <unordered_map>
#include <string>
#include <thread>
#include <memory>
#include <fmt/core.h>
#include <atomic>

class TokenBucket;

class BucketManager
{
private:
    std::mutex mapmutex;                                                   // mutex to lock the collection of buckets
    std::unordered_map<std::string, std::unique_ptr<TokenBucket>> buckets; // collection of buckets

    const double std_capacity = 1000;
    const double rate = 25;

    std::thread background_worker;               // thread to run cleanups
    std::atomic<bool> stop_cleanup_flag = false; // flag that runcleanupthread function depends on to run

    void runcleanupthread(); // function to run the cleanup thread

public:
    BucketManager(); // contructor inititazes values and initiazes cleanup thread

    ~BucketManager(); // ditructor , deallocates all resources and removes th cleanup thread

    // function for general bucket operations
    TokenBucket *GetBucket(const std::string &key,
                           double capacity = 0,
                           double rate = 0.0);
};