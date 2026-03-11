#pragma once
#include "Order.h"
#include "Trade.h"
#include "ObjectPool.h"
#include "OrderList.h"
#include <map>
#include <vector>

class OrderBook {
public:
    // Initialize with a pre-allocated pool size
    OrderBook(size_t poolSize = 100000) : orderPool_(poolSize) {}

    // Adds an order and returns any trades generated
    std::vector<Trade> addOrder(Order order);
    
    // Prints the current state of the order book
    void printBook() const;

private:
    std::vector<Trade> matchOrder(Order& order);

    ObjectPool<Order> orderPool_;

    // Bids (Buy orders): highest price first
    std::map<uint32_t, OrderList, std::greater<uint32_t>> bids_;
    
    // Asks (Sell orders): lowest price first
    std::map<uint32_t, OrderList, std::less<uint32_t>> asks_;
};

