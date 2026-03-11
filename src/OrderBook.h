#pragma once
#include "Order.h"
#include "Trade.h"
#include <map>
#include <list>
#include <vector>

class OrderBook {
public:
    // Adds an order and returns any trades generated
    std::vector<Trade> addOrder(Order order);
    
    // Prints the current state of the order book
    void printBook() const;

private:
    std::vector<Trade> matchOrder(Order& order);

    // Bids (Buy orders): highest price first
    std::map<uint32_t, std::list<Order>, std::greater<uint32_t>> bids_;
    
    // Asks (Sell orders): lowest price first
    std::map<uint32_t, std::list<Order>, std::less<uint32_t>> asks_;
};
