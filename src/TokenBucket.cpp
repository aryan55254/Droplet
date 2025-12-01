#include "../include/TokenBucket.hpp"
#include <stdexcept>

// constructor
TokenBucket::TokenBucket(double c, double r)
    : mux(), // Initialize mutex
      capacity(c),
      rate(r),
      tokens(c),
      last_check_time(std::chrono::steady_clock::now())
{
    // make sure inputs are valid
    if (c <= 0 || r <= 0)
    {
        throw std::invalid_argument("Capacity and rate must be greater than zero.");
    }
}

// refill function  Recalculates tokens based on elapsed time
void TokenBucket::refill()
{
    auto now = std::chrono::steady_clock::now();
    auto time_elapsed = now - last_check_time;
    using Seconds = std::chrono::duration<double>;
    double elapsed_seconds = std::chrono::duration_cast<Seconds>(time_elapsed).count();
    // if no time passed update last cheked time and and exit.
    if (elapsed_seconds <= 0)
    {
        last_check_time = now;
        return;
    }
    // Calculate tokens
    double tokens_to_add = elapsed_seconds * rate;
    tokens = std::min(tokens + tokens_to_add, capacity);
    last_check_time = now;
}
// thread-safe gatekeeper.
bool TokenBucket::allow(int n)
{
    // Lock the function
    std::lock_guard<std::mutex> lock(mux);
    // Update state to current time
    refill();
    // Check and consume requested tokens (n).
    if (tokens >= n)
    {
        tokens -= n; // remove tokens from the bucket
        return true; // ALLOWED
    }

    return false; // not allowed
}