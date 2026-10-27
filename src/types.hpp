#pragma once

#include <chrono>
#include <atomic>

namespace OrderEngine {

enum class OrderSide { BUY, SELL };

struct Order {
    uint64_t id;
    OrderSide side;
    double price;
    uint32_t quantity;
    std::chrono::high_resolution_clock::time_point timestamp;
    
    Order() : id(0), side(OrderSide::BUY), price(0.0), quantity(0), 
              timestamp(std::chrono::high_resolution_clock::now()) {}
    
    Order(uint64_t id, OrderSide side, double price, uint32_t quantity)
        : id(id), side(side), price(price), quantity(quantity), 
          timestamp(std::chrono::high_resolution_clock::now()) {}
};

struct Trade {
    uint64_t buy_order_id;
    uint64_t sell_order_id;
    double price;
    uint32_t quantity;
    std::chrono::high_resolution_clock::time_point timestamp;
};

struct LatencyStats {
    std::atomic<uint64_t> total_orders{0};
    std::atomic<uint64_t> total_latency_ns{0};
    std::atomic<uint64_t> min_latency_ns{UINT64_MAX};
    std::atomic<uint64_t> max_latency_ns{0};
    
    void recordLatency(uint64_t latency_ns) {
        total_orders++;
        total_latency_ns += latency_ns;
        
        uint64_t current_min = min_latency_ns.load();
        while (latency_ns < current_min && 
               !min_latency_ns.compare_exchange_weak(current_min, latency_ns));
        
        uint64_t current_max = max_latency_ns.load();
        while (latency_ns > current_max && 
               !max_latency_ns.compare_exchange_weak(current_max, latency_ns));
    }
    
    double getAverageLatencyUs() const {
        uint64_t orders = total_orders.load();
        return orders > 0 ? (total_latency_ns.load() / 1000.0) / orders : 0.0;
    }
    
    double getMinLatencyUs() const {
        uint64_t min_ns = min_latency_ns.load();
        return min_ns != UINT64_MAX ? min_ns / 1000.0 : 0.0;
    }
    
    double getMaxLatencyUs() const {
        return max_latency_ns.load() / 1000.0;
    }
};

} // namespace OrderEngine
