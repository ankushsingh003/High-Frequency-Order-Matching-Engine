#include "OrderBook.h"
#include <iostream>
#include <algorithm>

std::vector<Trade> OrderBook::addOrder(Order order) {
    std::vector<Trade> trades = matchOrder(order);
    
    // If order is not fully filled, add it to the book
    if (order.quantity > 0) {
        if (order.side == Side::Buy) {
            bids_[order.price].push_back(order);
        } else {
            asks_[order.price].push_back(order);
        }
    }
    
    return trades;
}

std::vector<Trade> OrderBook::matchOrder(Order& order) {
    std::vector<Trade> trades;

    if (order.side == Side::Buy) {
        // Match against asks
        while (order.quantity > 0 && !asks_.empty()) {
            auto bestAskIt = asks_.begin();
            uint32_t bestAskPrice = bestAskIt->first;
            
            if (bestAskPrice > order.price) {
                break; // No more matching asks
            }

            auto& ordersAtLevel = bestAskIt->second;
            while (order.quantity > 0 && !ordersAtLevel.empty()) {
                auto& restingOrder = ordersAtLevel.front();
                uint32_t tradeQuantity = std::min(order.quantity, restingOrder.quantity);
                
                trades.push_back({restingOrder.orderId, order.orderId, restingOrder.price, tradeQuantity});
                
                order.quantity -= tradeQuantity;
                restingOrder.quantity -= tradeQuantity;
                
                if (restingOrder.quantity == 0) {
                    ordersAtLevel.pop_front();
                }
            }
            
            if (ordersAtLevel.empty()) {
                asks_.erase(bestAskIt);
            }
        }
    } else {
        // Match against bids
        while (order.quantity > 0 && !bids_.empty()) {
            auto bestBidIt = bids_.begin();
            uint32_t bestBidPrice = bestBidIt->first;
            
            if (bestBidPrice < order.price) {
                break; // No more matching bids
            }

            auto& ordersAtLevel = bestBidIt->second;
            while (order.quantity > 0 && !ordersAtLevel.empty()) {
                auto& restingOrder = ordersAtLevel.front();
                uint32_t tradeQuantity = std::min(order.quantity, restingOrder.quantity);
                
                trades.push_back({restingOrder.orderId, order.orderId, restingOrder.price, tradeQuantity});
                
                order.quantity -= tradeQuantity;
                restingOrder.quantity -= tradeQuantity;
                
                if (restingOrder.quantity == 0) {
                    ordersAtLevel.pop_front();
                }
            }
            
            if (ordersAtLevel.empty()) {
                bids_.erase(bestBidIt);
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
        for (const auto& o : it->second) totalQty += o.quantity;
        std::cout << "  Price: " << it->first << " | Qty: " << totalQty << std::endl;
    }
    
    std::cout << "Bids:" << std::endl;
    for (const auto& [price, orders] : bids_) {
        uint32_t totalQty = 0;
        for (const auto& o : orders) totalQty += o.quantity;
        std::cout << "  Price: " << price << " | Qty: " << totalQty << std::endl;
    }
    std::cout << "------------------" << std::endl;
}
