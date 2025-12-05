#include <fmt/core.h>
#include <chrono>
#include <thread>
#include "../include/TokenBucket.hpp"
#include "../include/BucketManager.hpp"

void basic_rate_limit_test()
{
    fmt::print("\n=== Rate Limiter Functionality Test ===\n");

    TokenBucket bucket(3, 1);

    fmt::print("Allow 1: {}\n", bucket.allow(1));
    fmt::print("Allow 1: {}\n", bucket.allow(1));
    fmt::print("Allow 1: {}\n", bucket.allow(1));
    fmt::print("Allow 1 (should fail): {}\n", bucket.allow(1));
    fmt::print("\nWaiting 2 seconds to allow refill...\n");

    std::this_thread::sleep_for(std::chrono::seconds(2));

    fmt::print("Allow after refill: {}\n", bucket.allow(1));
    fmt::print("Allow 1 again: {}\n", bucket.allow(1));
}

void benchmark_test()
{
    fmt::print("\n=== BenchMarking Test ===\n");

    TokenBucket bucket(1000000, 1000000);

    auto start = std::chrono::high_resolution_clock::now();

    int iterations = 500000;

    for (int i = 0; i < iterations; i++)
        bucket.allow(1);

    auto end = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    fmt::print("{} operations took: {} ms\n", iterations, ms);
}

int main()
{
    basic_rate_limit_test();
    benchmark_test();
    return 0;
}
