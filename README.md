# Droplet

Droplet is a high-performance, thread-safe Token Bucket rate limiter implemented in modern C++.  
It enforces a stable request rate per client while allowing short bursts and rejecting excess traffic.  
The design supports concurrent access, timed token refilling, and automatic cleanup of inactive rate-limiter instances.

---

## Features

- Token Bucket rate-limiting algorithm  
- Thread-safe implementation using mutexes and atomic signals  
- Per-client isolation via `BucketManager`  
- Automatic cleanup of inactive buckets  
- Benchmarks and stress tests included  
- Raw TCP demo server  
- Load-testing script using Node.js to simulate real-world traffic  



## Folder Structure

```

Droplet
├── demo
│   └── tcp.cpp                  # Minimal TCP server demonstrating usage
├── include
│   ├── BucketManager.hpp        # Manages multiple TokenBuckets mapped to client keys
│   └── TokenBucket.hpp          # Core token bucket rate-limiter implementation
├── node_spam_tests
│   └── test.js                  # Load test script using raw TCP sockets
├── src
│   ├── BucketManager.cpp
│   └── TokenBucket.cpp
├── tests
│   └── tests.cpp                # Basic benchmark and correctness tests
├── .gitignore
└── README.md

````

---

## Building

### Build tests

```sh
g++ -std=c++20 tests/tests.cpp src/*.cpp -pthread -lfmt -o test
./test
````

### Build demo TCP server

```sh
g++ -std=c++20 demo/tcp.cpp src/*.cpp -pthread -lfmt -o server
./server
```

---

## Running the Load Test

With the server running:

```sh
node node_spam_tests/test.js
```

This script simulates concurrent raw TCP connections and tracks accepted vs rate-limited requests.

---

## Benchmarks & Results

Several performance experiments were run to evaluate Droplet under realistic and extreme conditions.

### Functional Test (Single Thread)

```
Allow 1: true
Allow 1: true
Allow 1: true
Allow 1 (should fail): false
Waiting 2 seconds...
Allow after refill: true
```

Behavior matches expected Token Bucket correctness.

---

### Benchmark: 500,000 Local Operations

```
500000 operations took: ~38 ms
```

Approximate throughput:

```
~13,000,000 operations/second
```

---

### Raw TCP Stress Test (Single Client, Heavy Burst)

```
Requests:           20000
Concurrency:        200
Time:               ~0.85s
Requests/sec:       ~23419
Allowed:            14
Rate Limited:       19986
```

Observed behavior aligned with the configured bucket:

```
Capacity: 10 tokens
Refill rate: 5 tokens/sec
Test runtime ≈ 0.85s → ~10 + 4 expected allowed
```

Actual result: **14 allowed**.

---

