#pragma once

#include <map>
#include <memory>
#include <functional>
#include "types.hpp"

namespace OrderEngine {

class OrderBook {
public:
    using TradeCallback = std::function<void(const Trade&)>;
    
    OrderBook() = default;
    ~OrderBook() = default;
    
    void setTradeCallback(TradeCallback callback) {
        trade_callback_ = callback;
    }
    
    size_t getBuyOrdersCount() const { return buy_orders_.size(); }
    size_t getSellOrdersCount() const { return sell_orders_.size(); }
    
private:
    // Price-time priority maps
    std::multimap<double, std::unique_ptr<Order>, std::greater<double>> buy_orders_;
    std::multimap<double, std::unique_ptr<Order>> sell_orders_;
    
    TradeCallback trade_callback_;
};

} // namespace OrderEngine
