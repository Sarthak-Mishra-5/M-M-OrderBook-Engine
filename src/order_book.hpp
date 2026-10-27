#pragma once

#include <map>
#include <memory>
#include <atomic>
#include <chrono>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "types.hpp"
#include "memory_pool.hpp"

namespace OrderEngine {

class OrderBook {
public:
    using TradeCallback = std::function<void(const Trade&)>;
    
    OrderBook();
    ~OrderBook();
    
    void start();
    void stop();
    void submitOrder(std::unique_ptr<Order> order);
    void setTradeCallback(TradeCallback callback);
    
    size_t getBuyOrdersCount() const;
    size_t getSellOrdersCount() const;
    const LatencyStats& getLatencyStats() const { return latency_stats_; }
    
private:
    std::multimap<double, std::unique_ptr<Order>, std::greater<double>> buy_orders_;
    std::multimap<double, std::unique_ptr<Order>> sell_orders_;
    
    std::queue<std::unique_ptr<Order>> order_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    
    std::thread matching_thread_;
    std::atomic<bool> running_{false};
    
    TradeCallback trade_callback_;
    MemoryPool<Order> order_pool_;
    std::atomic<uint64_t> next_trade_id_{1};
    LatencyStats latency_stats_;
    
    void matchingThreadFunc();
    void processOrder(std::unique_ptr<Order> order);
    void matchOrders();
    void executeTrade(Order& buy_order, Order& sell_order, uint32_t quantity);
};

} // namespace OrderEngine
