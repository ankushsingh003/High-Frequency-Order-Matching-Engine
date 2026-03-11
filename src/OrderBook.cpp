#include "OrderBook.h"
#include <iostream>
#include <algorithm>

std::vector<Trade> OrderBook::addOrder(Order order) {
    // Allocate an order from the pool
    Order* activeOrder = orderPool_.acquire(order.orderId, order.side, order.type, order.price, order.quantity);
    if (!activeOrder) {
        std::cerr << "CRITICAL: Order pool exhausted!" << std::endl;
        return {};
    }

    std::vector<Trade> trades = matchOrder(*activeOrder);
    
    // If order is not fully filled, add it to the book
    if (activeOrder->quantity > 0) {
        if (activeOrder->side == Side::Buy) {
            bids_[activeOrder->price].push_back(activeOrder);
        } else {
            asks_[activeOrder->price].push_back(activeOrder);
        }
    } else {
        // Order fully filled, return to pool
        orderPool_.release(activeOrder);
    }
    
    return trades;
}

std::vector<Trade> OrderBook::matchOrder(Order& order) {
    std::vector<Trade> trades;

    if (order.side == Side::Buy) {
        auto askIt = asks_.begin();
        while (order.quantity > 0 && askIt != asks_.end()) {
            uint32_t bestAskPrice = askIt->first;
            
            if (bestAskPrice > order.price) {
                break; // No more matching asks
            }

            OrderList& ordersAtLevel = askIt->second;
            while (order.quantity > 0 && !ordersAtLevel.empty()) {
                Order* restingOrder = ordersAtLevel.front();
                uint32_t tradeQuantity = std::min(order.quantity, restingOrder->quantity);
                
                trades.push_back({restingOrder->orderId, order.orderId, restingOrder->price, tradeQuantity});
                
                order.quantity -= tradeQuantity;
                restingOrder->quantity -= tradeQuantity;
                
                if (restingOrder->quantity == 0) {
                    ordersAtLevel.pop_front();
                    orderPool_.release(restingOrder); // Return fully matched resting order to pool
                }
            }
            
            if (ordersAtLevel.empty()) {
                askIt = asks_.erase(askIt);
            } else {
                break; // Still quantity resting here
            }
        }
    } else {
        auto bidIt = bids_.begin();
        while (order.quantity > 0 && bidIt != bids_.end()) {
            uint32_t bestBidPrice = bidIt->first;
            
            if (bestBidPrice < order.price) {
                break; // No more matching bids
            }

            OrderList& ordersAtLevel = bidIt->second;
            while (order.quantity > 0 && !ordersAtLevel.empty()) {
                Order* restingOrder = ordersAtLevel.front();
                uint32_t tradeQuantity = std::min(order.quantity, restingOrder->quantity);
                
                trades.push_back({restingOrder->orderId, order.orderId, restingOrder->price, tradeQuantity});
                
                order.quantity -= tradeQuantity;
                restingOrder->quantity -= tradeQuantity;
                
                if (restingOrder->quantity == 0) {
                    ordersAtLevel.pop_front();
                    orderPool_.release(restingOrder); // Return fully matched resting order to pool
                }
            }
            
            if (ordersAtLevel.empty()) {
                bidIt = bids_.erase(bidIt);
            } else {
                break;
            }
        }
    }
    return trades;
}

void OrderBook::printBook() const {
    std::cout << "--- Order Book ---" << std::endl;
    std::cout << "Asks:" << std::endl;
    for (auto it = asks_.rbegin(); it != asks_.rend(); ++it) {
        uint32_t totalQty = 0;
        Order* current = it->second.front();
        while (current) {
            totalQty += current->quantity;
            current = current->next;
        }
        std::cout << "  Price: " << it->first << " | Qty: " << totalQty << std::endl;
    }
    
    std::cout << "Bids:" << std::endl;
    for (const auto& [price, orderList] : bids_) {
        uint32_t totalQty = 0;
        Order* current = orderList.front();
        while (current) {
            totalQty += current->quantity;
            current = current->next;
        }
        std::cout << "  Price: " << price << " | Qty: " << totalQty << std::endl;
    }
    std::cout << "------------------" << std::endl;
}

