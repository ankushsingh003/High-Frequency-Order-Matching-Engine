#include "OrderBook.h"
#include "SPSCQueue.h"
#include "BinaryProtocol.h"
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
            if (!trades.empty()) {
                std::cout << "[Engine] Matched " << trades.size() << " trades." << std::endl;
            }
        } else {
            // Spin-wait / yield
            std::this_thread::yield();
        }
    }
}

int main() {
    std::cout << "High-Frequency Order Matching Engine Initialized" << std::endl;
    std::cout << "Starting Concurrency Model with Binary Protocol..." << std::endl;
    
    SPSCQueue<Order> orderQueue(1024);
    
    // Start matching thread
    std::thread engine(matchingEngineThread, std::ref(orderQueue));
    
    uint64_t nextOrderId = 1;

    // Simulate receiving binary packets over the network
    std::cout << "Simulating incoming binary order packets..." << std::endl;

    // Packet 1: Buy Limit, 100 @ 10
    BinaryOrderMessage packet1 = {'O', static_cast<uint8_t>(Side::Buy), static_cast<uint8_t>(OrderType::Limit), 100, 10};
    
    // Packet 2: Sell Limit, 100 @ 5
    BinaryOrderMessage packet2 = {'O', static_cast<uint8_t>(Side::Sell), static_cast<uint8_t>(OrderType::Limit), 100, 5};
    
    // Packet 3: Sell Fill-Or-Kill, 105 @ 5 (Should fail/cancel because Best Bid is 100)
    BinaryOrderMessage packet3 = {'O', static_cast<uint8_t>(Side::Sell), static_cast<uint8_t>(OrderType::FOK), 105, 5};

    const BinaryOrderMessage* incomingPackets[] = {&packet1, &packet2, &packet3};

    for (const auto* packet : incomingPackets) {
        Order parsedOrder;
        // Zero-allocation parse payload bytes directly to standard object
        if (BinaryProtocolParser::parse(reinterpret_cast<const uint8_t*>(packet), sizeof(BinaryOrderMessage), parsedOrder, nextOrderId++)) {
            while (!orderQueue.push(parsedOrder)) {
                std::this_thread::yield();
            }
            std::cout << "[Network] Dispatched Order " << parsedOrder.orderId 
                      << " (Side: " << (int)parsedOrder.side << ", Type: " << (int)parsedOrder.type << ")" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate slight network delay
    }

    // Allow engine to process before shutting down
    std::this_thread::sleep_for(std::chrono::seconds(1));

    running.store(false, std::memory_order_relaxed);
    engine.join();
    
    std::cout << "Engine shut down cleanly." << std::endl;
    return 0;
}



