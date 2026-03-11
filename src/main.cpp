#include "OrderBook.h"
#include <iostream>

int main() {
    std::cout << "High-Frequency Order Matching Engine Initialized" << std::endl;
    
    OrderBook book;
    
    // Add some initial orders
    book.addOrder(Order(1, Side::Buy, OrderType::Limit, 100, 10));
    book.addOrder(Order(2, Side::Sell, OrderType::Limit, 105, 5));
    book.printBook();
    
    std::cout << "Adding crossing order..." << std::endl;
    // Add a crossing order that matches partially with the ask and rests the remaining part
    auto trades = book.addOrder(Order(3, Side::Buy, OrderType::Limit, 105, 7));
    
    for (const auto& t : trades) {
        std::cout << "Trade: Maker " << t.makerOrderId << " Taker " << t.takerOrderId 
                  << " Price " << t.price << " Qty " << t.quantity << std::endl;
    }
    
    book.printBook();

    return 0;
}

