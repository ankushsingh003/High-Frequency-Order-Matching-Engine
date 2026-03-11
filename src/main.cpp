#include "OrderBook.h"
#include "SPSCQueue.h"
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>

std::atomic<bool> running{true};

void matchingEngineThread(SPSCQueue<Order>& orderQueue) {
    OrderBook book;
    Order order;
    
    while (running.load(std::memory_order_relaxed)) {
        if (orderQueue.pop(order)) {
            // Process order
            auto trades = book.addOrder(order);
            // In a real system, trades would be pushed to an outgoing queue
        } else {
            // Spin-wait / yield
            std::this_thread::yield();
        }
    }
}

int main() {
    std::cout << "High-Frequency Order Matching Engine Initialized" << std::endl;
    std::cout << "Starting Concurrency Model..." << std::endl;
    
    SPSCQueue<Order> orderQueue(1024);
    
    // Start matching thread
    std::thread engine(matchingEngineThread, std::ref(orderQueue));
    
    std::cout << "Commands: BUY <price> <qty>, SELL <price> <qty>, EXIT" << std::endl;
    
    uint64_t nextOrderId = 1;
    std::string command;
    while (std::cin >> command) {
        if (command == "EXIT") {
            break;
        } else if (command == "BUY" || command == "SELL") {
            uint32_t price, quantity;
            if (!(std::cin >> price >> quantity)) {
                std::cout << "Invalid format." << std::endl;
                std::cin.clear();
                break;
            }
            
            Side side = (command == "BUY") ? Side::Buy : Side::Sell;
            Order order(nextOrderId++, side, OrderType::Limit, price, quantity);
            
            // Push to lock-free queue
            while (!orderQueue.push(order)) {
                std::this_thread::yield(); // Wait if full
            }
            std::cout << "Order dispatched to engine." << std::endl;
        }
    }

    running.store(false, std::memory_order_relaxed);
    engine.join();
    
    std::cout << "Engine shut down cleanly." << std::endl;
    return 0;
}


