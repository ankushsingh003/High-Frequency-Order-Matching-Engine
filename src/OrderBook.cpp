#include "OrderBook.h"
#include <iostream>
#include <algorithm>

bool OrderBook::canFillCompletely(Side side, uint32_t price, uint32_t quantity) const {
    uint32_t filled = 0;
    
    if (side == Side::Buy) {
        for (const auto& [askPrice, orderList] : asks_) {
            if (askPrice > price) break;
            
            Order* current = orderList.front();
            while (current) {
                filled += current->quantity;
                if (filled >= quantity) return true;
                current = current->next;
            }
        }
    } else {
        for (const auto& [bidPrice, orderList] : bids_) {
            if (bidPrice < price) break;
            
            Order* current = orderList.front();
            while (current) {
                filled += current->quantity;
                if (filled >= quantity) return true;
                current = current->next;
            }
        }
    }
    return false;
}

std::vector<Trade> OrderBook::addOrder(Order order) {
    // If it's a FOK order, we must verify the liquidity exists upfront.
    if (order.type == OrderType::FOK) {
        if (!canFillCompletely(order.side, order.price, order.quantity)) {
            // Cancelled entirely, no trades
            return {};
        }
        // If we reach here, we know it can fill completely, so we process it like IOC.
        order.type = OrderType::IOC; 
    }

    // Allocate an order from the pool
    Order* activeOrder = orderPool_.acquire(order.orderId, order.side, order.type, order.price, order.quantity);
    if (!activeOrder) {
        std::cerr << "CRITICAL: Order pool exhausted!" << std::endl;
        return {};
    }

    std::vector<Trade> trades = matchOrder(*activeOrder);
    
    // If order is not fully filled, check its type before adding to the book
    if (activeOrder->quantity > 0) {
        if (activeOrder->type == OrderType::IOC || activeOrder->type == OrderType::FOK) {
            // Cancel remaining quantity (do not add to resting book)
            orderPool_.release(activeOrder);
        } else {
            // Limit order: Add resting quantity to the book
            if (activeOrder->side == Side::Buy) {
                bids_[activeOrder->price].push_back(activeOrder);
            } else {
                asks_[activeOrder->price].push_back(activeOrder);
            }
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
            
            if (bestAskPrice > order.price && order.type != OrderType::Market) {
                break; // No more matching asks within limits
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
            
            if (bestBidPrice < order.price && order.type != OrderType::Market) {
                break; // No more matching bids within limits
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

