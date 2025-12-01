#include <fmt/core.h>
#include <chrono>
#include <mutex>
#include <cstdint>
#include <algorithm>

class TokenBucket
{
private:
    std::mutex mux;
    double tokens;                                         // tokens present currently in the bucket
    std::chrono::steady_clock::time_point last_check_time; //  last time bucket was checked to be filled
    double capacity;                                       // capacity of the bucket
    double rate;                                           // tokens per second

    // function to refill the bucket with per second rate prieciion

    void refill()
    {
        auto now = std::chrono::steady_clock::now();   // the time at which refill was called
        auto time_elapsed = now - last_check_time;     // time b/w current and last refill calls
        using Seconds = std::chrono::duration<double>; // converting the time to seconds
        double elapsed_seconds = std::chrono::duration_cast<Seconds>(time_elapsed).count();
        // make sure we don't proceed if time hasn't passed
        if (elapsed_seconds <= 0)
        {
            last_check_time = now; // update the last check time and break
            return;
        }
        double tokens_to_add = elapsed_seconds * rate;       // tokens to add acc to rate
        tokens = std::min(tokens_to_add + tokens, capacity); // update the tokens
        last_check_time = now;                               // update the last check time
    };

public:
    // cunstructor to intitiaze and declare values
    TokenBucket(double c, double r) : mux(), capacity(c), rate(r), tokens(c), last_check_time(std::chrono::steady_clock::now()) {};
    // function to allow clients interact with bucket

    bool allow(int n)
    {
        std::lock_guard<std::mutex> lock(mux); // lock the function
        refill();                              // call refill and fill accordingly
        if (tokens >= n)
        {
            tokens = tokens - n; // remove one token each time client sents
            return true;         // allow to access server
        }
        return false; // don't allow
    }
};