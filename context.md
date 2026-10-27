# Context: M&M OrderBook Engine

## Project Overview

**M&M OrderBook Engine** (Management & Matching) is a small-scale, high-performance limit order book engine designed to simulate exchange-level order matching for a high-frequency trading (HFT) system. It processes real-time buy and sell orders with ultra-low latency, targeting sub-100µs for order matching.

The name "M&M" embodies the dual responsibilities of any order book engine:
- **Management**: Efficient order storage, memory management, and state tracking
- **Matching**: Fast price-time priority order execution

The project is implemented in C++17, leveraging multithreading, lock-free data structures, and custom memory management to achieve high performance. It supports a FIX-like JSON-based protocol for order input and logs trade data for analysis. The project is intended for a resume, showcasing skills in low-latency systems, concurrent programming, and financial systems.

## Project Structure

```
mm-orderbook-engine/
├── src/
│   ├── main.cpp              # Entry point and main application logic
│   ├── order_book.hpp        # Order book class declaration
│   ├── order_book.cpp        # Order book implementation
│   ├── parser.hpp            # JSON order message parser
│   ├── parser.cpp            # Parser implementation
│   ├── logger.hpp            # Trade logging system
│   ├── logger.cpp            # Logger implementation
│   └── memory_pool.hpp       # Custom memory pool for orders
├── tests/
│   └── test_order_book.cpp   # Unit tests for order book
├── CMakeLists.txt            # Build configuration
└── context.md                # Project documentation
```

## Key Features

- **Multithreaded Architecture**: Separate threads for order input, matching, and logging to maximize throughput.
- **Lock-Free Data Structures**: Use lock-free techniques to minimize contention and ensure thread safety.
- **Custom Memory Pools**: Reduce dynamic memory allocation overhead for order objects.
- **Priority Queues**: Implement buy/sell order books using std::multimap for O(log N) insertion and removal.
- **FIX-like Protocol Parser**: Parse JSON-based order messages (e.g., buy/sell, price, quantity).
- **Trade Logging**: Record executed trades and order flow in a time-series format for post-trade analysis.
- **Low Latency**: Target sub-100µs latency for order matching under typical workloads.

## The M&M Philosophy

Like a well-orchestrated performance, M&M OrderBook Engine harmonizes two critical functions:
- **Management**: Keeping order books organized, memory pools efficient, and state consistent
- **Matching**: Executing trades with precision and speed that rivals the best in the industry

The project draws inspiration from the relentless pursuit of excellence in both trading systems and performance art — where every microsecond counts and every trade matters.

## Scope and Constraints

- **Small Scale**: Designed to be completed in ~1 week by a single developer.
- **Simplified Functionality**: Focuses on core order matching (limit orders only, no market orders or advanced features like order cancellation).
- **Input**: JSON-based order messages via a simple TCP socket or file-based input for testing.
- **Output**: Console output for matched trades and a CSV log file for trade data.
- **Deployment**: Host a demo on a public server (e.g., AWS EC2) with a simple API or script to simulate order streams and display results.

## Target Audience

- **Hiring Managers/Recruiters**: Demonstrates expertise in C++17, low-latency programming, multithreading, and financial systems.
- **Public Viewers**: A live demo on a cloud server to showcase functionality via a simple interface or log output.

## Technology Stack

- **Language**: C++17
- **Libraries**: Standard Template Library (STL) for std::multimap, std::atomic for lock-free operations, and JSON parsing (e.g., nlohmann/json).
- **Build Tools**: CMake for cross-platform builds.
- **Testing**: Unit tests with Catch2 or Google Test.
- **Deployment**: AWS EC2 or similar for public demo, with a TCP socket server for order input.

## Success Criteria

- Achieve sub-100µs latency for order matching (measured on a standard laptop).
- Correctly match buy/sell orders based on price-time priority.
- Parse and process JSON orders without errors.
- Log trades in a CSV file with timestamps, price, and quantity.
- Deploy a live demo accessible via a public IP, showing real-time order matching.