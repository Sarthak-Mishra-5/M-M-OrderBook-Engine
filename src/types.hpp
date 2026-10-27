#pragma once

#include <chrono>

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

} // namespace OrderEngine
