#pragma once
#include "Order.h"
#include "Trade.h"
#include "ObjectPool.h"
#include "OrderList.h"
#include <map>
#include <vector>

class OrderBook {
public:
    
    OrderBook(size_t poolSize = 100000) : orderPool_(poolSize) {}

    
    std::vector<Trade> addOrder(Order order);
    
    
    void printBook() const;

private:
    std::vector<Trade> matchOrder(Order& order);
    bool canFillCompletely(Side side, uint32_t price, uint32_t quantity) const;

    ObjectPool<Order> orderPool_;

    
    std::map<uint32_t, OrderList, std::greater<uint32_t>> bids_;
    
    
    std::map<uint32_t, OrderList, std::less<uint32_t>> asks_;
};

