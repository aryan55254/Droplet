#pragma once

#include <mutex>
#include <unordered_map>
#include <string>
#include <thread>
#include <memory>
#include <fmt/core.h>
#include <atomic>
#include <condition_variable>

class TokenBucket;

class BucketManager
{
private:
    std::mutex mapmutex;                                                   // mutex to lock the collection of buckets
    std::unordered_map<std::string, std::shared_ptr<TokenBucket>> buckets; // collection of buckets

    const double std_capacity = 1000;
    const double rate = 5;

    std::thread background_worker;               // thread to run cleanups
    std::atomic<bool> stop_cleanup_flag = false; // flag that runcleanupthread function depends on to run

    std::mutex cv_mutex;
    std::condition_variable cv;

    void runcleanupthread(); // function to run the cleanup thread

public:
    BucketManager(); // contructor inititazes values and initiazes cleanup thread

    ~BucketManager(); // ditructor , deallocates all resources and removes th cleanup thread

    // function for general bucket operations
    std::shared_ptr<TokenBucket> GetBucket(const std::string &key,
                                           double capacity,
                                           double rate);
};