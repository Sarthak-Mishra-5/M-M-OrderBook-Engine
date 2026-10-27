#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include "order_book.hpp"
#include "parser.hpp"
#include "logger.hpp"

using namespace OrderEngine;

class OrderBookServer {
public:
    OrderBookServer(int port = 8080) : port_(port) {}
    
    void start() {
        order_book_.setTradeCallback([this](const Trade& trade) {
            logger_.logTrade(trade);
            std::cout << "TRADE: Buy Order " << trade.buy_order_id 
                      << " matched with Sell Order " << trade.sell_order_id
                      << " at price " << trade.price 
                      << " for quantity " << trade.quantity << "\n";
            total_trades_++;
        });
        
        logger_.start();
        order_book_.start();
        
        std::cout << "Ultra-Low Latency Order Book Engine Starting...\n";
        std::cout << "Server listening on port " << port_ << "\n";
        
        std::thread console_thread(&OrderBookServer::consoleInputThread, this);
        std::thread stats_thread(&OrderBookServer::statsThread, this);
        std::thread tcp_thread(&OrderBookServer::tcpServerThread, this);
        
        console_thread.join();
        
        running_ = false;
        if (stats_thread.joinable()) stats_thread.join();
        if (tcp_thread.joinable()) tcp_thread.join();
        
        order_book_.stop();
        logger_.stop();
    }
    
private:
    OrderBook order_book_;
    OrderParser parser_;
    TradeLogger logger_{"trades.csv"};
    int port_;
    std::atomic<bool> running_{true};
    std::atomic<uint64_t> total_trades_{0};
    
    void consoleInputThread() {
        std::cout << "Commands: 'quit', 'stats', or JSON orders\n";
        std::cout << "Example: {\"side\":\"buy\",\"price\":100.50,\"quantity\":10}\n\n";
        
        std::string input;
        while (std::getline(std::cin, input) && running_) {
            if (input == "quit" || input == "exit") {
                break;
            } else if (input == "stats") {
                printStats();
            } else if (!input.empty()) {
                processOrderString(input);
            }
        }
    }
    
    void processOrderString(const std::string& order_str) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        auto order = parser_.parseOrder(order_str);
        if (order) {
            order_book_.submitOrder(std::move(*order));
        } else {
            std::cout << "Error: Invalid order format\n";
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - start_time);
        
        std::cout << "Input processing: " << duration.count() << "µs | "
                  << "Buy orders: " << order_book_.getBuyOrdersCount() << " | "
                  << "Sell orders: " << order_book_.getSellOrdersCount() << "\n";
    }
    
    void statsThread() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            if (running_) {
                printStats();
            }
        }
    }
    
    void printStats() {
        const auto& stats = order_book_.getLatencyStats();
        std::cout << "\n=== ORDER BOOK STATISTICS ===\n";
        std::cout << "Total Orders Processed: " << stats.total_orders << "\n";
        std::cout << "Total Trades Executed: " << total_trades_ << "\n";
        std::cout << "Average Latency: " << std::fixed << std::setprecision(2) 
                  << stats.getAverageLatencyUs() << "µs\n";
        std::cout << "Min Latency: " << stats.getMinLatencyUs() << "µs\n";
        std::cout << "Max Latency: " << stats.getMaxLatencyUs() << "µs\n";
        std::cout << "Active Buy Orders: " << order_book_.getBuyOrdersCount() << "\n";
        std::cout << "Active Sell Orders: " << order_book_.getSellOrdersCount() << "\n";
        std::cout << "============================\n\n";
    }
    
    void tcpServerThread() {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
            std::cerr << "Failed to create socket\n";
            return;
        }
        
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port_);
        
        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "Bind failed\n";
            close(server_fd);
            return;
        }
        
        if (listen(server_fd, 10) < 0) {
            std::cerr << "Listen failed\n";
            close(server_fd);
            return;
        }
        
        std::cout << "TCP server ready on
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.12)
project(OrderBookEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")

find_package(Threads REQUIRED)
include_directories(src)

set(SOURCES
    src/order_book.cpp
    src/parser.cpp
    src/logger.cpp
)

add_executable(order_engine src/main.cpp ${SOURCES})
target_link_libraries(order_engine Threads::Threads)

file(WRITE ${CMAKE_BINARY_DIR}/run_demo.sh 
"#!/bin/bash\n"
"echo 'Starting Order Book Engine Demo...'\n"
"echo 'Connect via: telnet localhost 8080'\n"
"echo 'Send JSON orders or use console commands'\n"
"./order_engine 8080\n"
)

execute_process(COMMAND chmod +x ${CMAKE_BINARY_DIR}/run_demo.sh)
