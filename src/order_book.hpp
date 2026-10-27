#pragma once

#include <map>
#include <memory>
#include <functional>
#include "types.hpp"
#include "memory_pool.hpp"

namespace OrderEngine {

class OrderBook {
public:
    using TradeCallback = std::function<void(const Trade&)>;
    
    OrderBook() = default;
    ~OrderBook() = default;
    
    void setTradeCallback(TradeCallback callback) {
        trade_callback_ = callback;
    }
    
    void submitOrder(std::unique_ptr<Order> order);
    
    size_t getBuyOrdersCount() const { return buy_orders_.size(); }
    size_t getSellOrdersCount() const { return sell_orders_.size(); }
    
private:
    std::multimap<double, std::unique_ptr<Order>, std::greater<double>> buy_orders_;
    std::multimap<double, std::unique_ptr<Order>> sell_orders_;
    
    TradeCallback trade_callback_;
    MemoryPool<Order> order_pool_;
    std::atomic<uint64_t> next_trade_id_{1};
    
    void processOrder(std::unique_ptr<Order> order);
    void matchOrders();
    void executeTrade(Order& buy_order, Order& sell_order, uint32_t quantity);
};
