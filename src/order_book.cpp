#include "order_book.hpp"

namespace OrderEngine {

OrderBook::OrderBook() = default;

OrderBook::~OrderBook() {
    stop();
}

void OrderBook::start() {
    running_ = true;
    matching_thread_ = std::thread(&OrderBook::matchingThreadFunc, this);
}

void OrderBook::stop() {
    running_ = false;
    queue_cv_.notify_all();
    if (matching_thread_.joinable()) {
        matching_thread_.join();
    }
}

void OrderBook::submitOrder(std::unique_ptr<Order> order) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    order_queue_.push(std::move(order));
    queue_cv_.notify_one();
}

void OrderBook::setTradeCallback(TradeCallback callback) {
    trade_callback_ = callback;
}

size_t OrderBook::getBuyOrdersCount() const {
    return buy_orders_.size();
}

size_t OrderBook::getSellOrdersCount() const {
    return sell_orders_.size();
}

void OrderBook::matchingThreadFunc() {
    while (running_) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        queue_cv_.wait(lock, [this] { return !order_queue_.empty() || !running_; });
        
        if (!running_) break;
        
        while (!order_queue_.empty()) {
            auto order = std::move(order_queue_.front());
            order_queue_.pop();
            lock.unlock();
            
            auto start = std::chrono::high_resolution_clock::now();
            processOrder(std::move(order));
            auto end = std::chrono::high_resolution_clock::now();
            
            auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            latency_stats_.recordLatency(latency);
            
            lock.lock();
        }
    }
}

void OrderBook::processOrder(std::unique_ptr<Order> order) {
    if (order->side == OrderSide::BUY) {
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
