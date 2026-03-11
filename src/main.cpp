#include "OrderBook.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "High-Frequency Order Matching Engine Initialized" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  BUY <price> <qty>" << std::endl;
    std::cout << "  SELL <price> <qty>" << std::endl;
    std::cout << "  PRINT" << std::endl;
    std::cout << "  EXIT" << std::endl;
    
    OrderBook book;
    uint64_t nextOrderId = 1;

    std::string command;
    while (std::cin >> command) {
        if (command == "EXIT") {
            break;
        } else if (command == "PRINT") {
            book.printBook();
        } else if (command == "BUY" || command == "SELL") {
            uint32_t price, quantity;
            if (!(std::cin >> price >> quantity)) {
                std::cout << "Invalid format. Use: <BUY/SELL> <price> <qty>" << std::endl;
                std::cin.clear();
                break;
            }
            
            Side side = (command == "BUY") ? Side::Buy : Side::Sell;
            Order order(nextOrderId++, side, OrderType::Limit, price, quantity);
            auto trades = book.addOrder(order);
            
            if (trades.empty()) {
                std::cout << "Order added to book." << std::endl;
            } else {
                for (const auto& t : trades) {
                    std::cout << "TRADE: Maker " << t.makerOrderId << " Taker " << t.takerOrderId 
                              << " Price " << t.price << " Qty " << t.quantity << std::endl;
                }
            }
        } else {
            std::cout << "Unknown command." << std::endl;
        }
    }

    return 0;
}

