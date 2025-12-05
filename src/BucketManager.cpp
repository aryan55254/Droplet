#include "../include/BucketManager.hpp"
#include "../include/TokenBucket.hpp"
#include <stdexcept>
#include <chrono>
#include <thread>
#include <fmt/core.h>
#include <atomic>
#include <memory>

using Clock = std::chrono::steady_clock;
BucketManager::BucketManager()
    : mapmutex(),
      buckets(),
      std_capacity(1000.0),
      rate(5.0),
      stop_cleanup_flag(false),
      cv_mutex(),
      cv()

{
    background_worker = std::thread(&BucketManager::runcleanupthread, this);
    fmt::print("Cleanup Cron Jobs Started");
}

BucketManager::~BucketManager()
{

    stop_cleanup_flag = true;
    cv.notify_one();
    if (background_worker.joinable())
    {
        background_worker.join();
    }
    fmt::print("BucketManager shutdown complete.\n");
}

std::shared_ptr<TokenBucket> BucketManager::GetBucket(const std::string &key, double capacity, double rate)
{
    std::lock_guard<std::mutex> lock(mapmutex);

    auto it = buckets.find(key);
    if (it != buckets.end())
    {
        return it->second;
    }

    auto new_bucket = std::make_shared<TokenBucket>(capacity, rate);
    buckets[key] = new_bucket;
    return new_bucket;
}

void BucketManager::runcleanupthread()
{
    const auto CLEANUP_INTERVAL = std::chrono::minutes(30);
    const auto STALE_THRESHOLD = std::chrono::hours(24);
    while (!stop_cleanup_flag)
    {
        std::unique_lock<std::mutex> cv_lock(cv_mutex);
        cv.wait_for(cv_lock, CLEANUP_INTERVAL, [this]
                    { return stop_cleanup_flag.load(); });
        if (stop_cleanup_flag)
            break;
        cv_lock.unlock();
        auto now = std::chrono::steady_clock::now();
        std::lock_guard<std::mutex> map_lock(mapmutex);
        for (auto it = buckets.begin(); it != buckets.end();)
        {
            TimePoint last_seen = it->second->getLastAccessedTime();
            auto duration = now - last_seen;

            if (duration > STALE_THRESHOLD)
            {

                it = buckets.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}