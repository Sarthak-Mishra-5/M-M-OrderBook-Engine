#include "order_book.hpp"
#include <atomic>

namespace OrderEngine {

void OrderBook::submitOrder(std::unique_ptr<Order> order) {
    processOrder(std::move(order));
}

void OrderBook::processOrder(std::unique_ptr<Order> order) {
    if (order->side == OrderSide::BUY) {
        matchOrders();
        
        auto it = sell_orders_.begin();
        while (it != sell_orders_.end() && order->quantity > 0) {
            if (order->price >= it->first) {
                uint32_t match_qty = std::min(order->quantity, it->second->quantity);
                executeTrade(*order, *it->second, match_qty);
                
                if (it->second->quantity == 0) {
                    it = sell_orders_.erase(it);
                } else {
                    ++it;
                }
            } else {
                break;
            }
        }
        
        if (order->quantity > 0) {
            buy_orders_.insert({order->price, std::move(order)});
        }
    } else {
        matchOrders();
        
        auto it = buy_orders_.begin();
        while (it != buy_orders_.end() && order->quantity > 0) {
            if (order->price <= it->first) {
                uint32_t match_qty = std::min(order->quantity, it->second->quantity);
                executeTrade(*it->second, *order, match_qty);
                
                if (it->second->quantity == 0) {
                    it = buy_orders_.erase(it);
                } else {
                    ++it;
                }
            } else {
                break;
            }
        }
        
        if (order->quantity > 0) {
            sell_orders_.insert({order->price, std::move(order)});
        }
    }
}

void OrderBook::matchOrders() {
    // Matching logic integrated in processOrder
}

void OrderBook::executeTrade(Order& buy_order, Order& sell_order, uint32_t quantity) {
    buy_order.quantity -= quantity;
    sell_order.quantity -= quantity;
    
    if (trade_callback_) {
        Trade trade;
        trade.buy_order_id = buy_order.id;
        trade.sell_order_id = sell_order.id;
        trade.price = sell_order.price;
        trade.quantity = quantity;
        trade.timestamp = std::chrono::high_resolution_clock::now();
        trade_callback_(trade);
    }
}

} // namespace OrderEngine
